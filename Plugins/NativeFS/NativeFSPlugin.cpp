// system headers
#include <algorithm>

// library headers
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/FileIO/FSAPI.h>

// plugin headers
#include "NativeFSUtils.h"

static Logger logger = init_stderr_logger("NativeFS", LogLevel::trace);

static inline Logger get_logger() { return logger; }

static Vector<FileLoaderHandle> g_loaders;

// strip a prefix from path if present. Returns true if stripped.
static bool strip_prefix(String& path, const String& prefix)
{
    // path and prefix are normalized with '/'
    if (path.size() < prefix.size())
        return false;

    if (path.compare(0, prefix.size(), prefix) != 0)
        return false;

    // either exact match or next char is '/'
    if (path.size() > prefix.size() && path[prefix.size()] != '/')
        return false;

    // strip prefix and optional '/'
    if (path.size() == prefix.size()) {
        path = ""; // requesting the mount root
    } else {
        path.erase(0, prefix.size());
        if (!path.empty() && path.front() == '/')
            path.erase(0, 1);
    }
    return true;
}

// normalize virtual path: ensure leading '/', strip trailing '/'
static String normalize_vpath(FSPath vpath)
{
    if (!vpath) return String("/");

    String s(vpath);
    if (s.empty()) return String("/");

    // replace backslashes with forward slashes to be consistent
    std::replace(s.begin(), s.end(), '\\', '/');
    if (s.front() != '/')
        s.insert(s.begin(), '/');

    // remove trailing '/'
    while (s.size() > 1 && s.back() == '/')
        s.pop_back();

    return s;
}

static bool sort_mount_points(const NativeMount& a, const NativeMount& b)
{
    // sort based on priority
    if (a.priority != b.priority)
        return a.priority > b.priority;

    // longer vpath first as tiebreaker
    return a.vpath.size() > b.vpath.size();
}

// resolve a VFS path to a list of real OS paths in search order (read side).
// if the incoming path is absolute on the OS, return it directly.
static Vector<fs::path> resolve_read_paths(NativeFSLoader* loader, FSPath cpath)
{
    Vector<fs::path> out;
    if (!cpath) return out;

    String path(cpath);
    if (path.empty()) return out;

    // normalize slashes
    std::replace(path.begin(), path.end(), '\\', '/');

    // absolute OS path? Let caller use it directly.
    fs::path probe(path);
    if (probe.is_absolute()) {
        out.push_back(probe);
        return out;
    }

    // collect candidate real paths from mounts that match the longest prefix, by priority.
    std::lock_guard<std::mutex> lock(loader->mounts_mutex);

    // sort mounts by priority (desc) every time only if needed; cheap for small N
    Vector<NativeMount> mounts = loader->mounts;
    std::sort(mounts.begin(), mounts.end(),
        [](const NativeMount& a, const NativeMount& b) {
        if (a.priority != b.priority)
            return a.priority > b.priority;
        return a.vpath.size() > b.vpath.size(); // longer vpath first as tiebreaker
    });

    // check file path validity
    for (const auto& m : mounts) {
        String sub = path;
        if (!m.vpath.empty() && m.vpath != "/")
            if (!strip_prefix(sub, m.vpath))
                continue; // not under this mount

        fs::path real = m.root;
        if (!sub.empty())
            real /= fs::path(sub);
        out.push_back(real);
    }
    return out;
}

// -----------------------------------------------------------------------------
// API functions
// -----------------------------------------------------------------------------

static CString get_api_name()
{
    return "NativeFS";
}

static bool create_loader(FileLoaderHandle& loader)
{
    auto ptr       = new NativeFSLoader();
    loader.pointer = ptr;
    if (loader.valid()) {
        g_loaders.push_back(loader);
    }
    return loader.valid();
}

static bool delete_loader(FileLoaderHandle loader)
{
    if (!loader.valid())
        return false;

    // remove from loaders
    g_loaders.erase(
        std::remove(g_loaders.begin(), g_loaders.end(), loader),
        g_loaders.end());

    // delete pointer
    auto pointer = loader.astype<NativeFSLoader>();
    delete pointer;
    return true;
}

static size_t sizeof_file(FileLoaderHandle loader, FSPath path)
{
    if (!path) {
        get_logger()->error("sizeof_file: input path is null!");
        return 0;
    }

    auto candidates = resolve_read_paths(loader.astype<NativeFSLoader>(), path);
    for (const auto& p : candidates) {
        std::error_code ec;
        if (fs::exists(p, ec) && fs::is_regular_file(p, ec)) {
            auto sz = fs::file_size(p, ec);
            if (!ec) {
                get_logger()->trace("sizeof_file: {} -> {} bytes", path, static_cast<unsigned long long>(sz));
                return sz;
            }
        }
    }
    return 0;
}

static bool exists_file(FileLoaderHandle loader, FSPath path)
{
    if (!path) {
        get_logger()->error("exists_file: input path is null!");
        return false;
    }

    auto candidates = resolve_read_paths(loader.astype<NativeFSLoader>(), path);
    for (const auto& p : candidates) {
        std::error_code ec;
        if (fs::exists(p, ec) && fs::is_regular_file(p, ec))
            return true;
    }
    return false;
}

static bool open_file(FileLoaderHandle loader, FileHandle& out_handle, FSPath path)
{
    if (!path) {
        get_logger()->error("open_file: input path is null!");
        return false;
    }

    auto pointer = loader.astype<NativeFSLoader>();

    // assign a new file index
    uint file_index = pointer->file_index++;

    // find and open the corresponding file
    auto candidates = resolve_read_paths(pointer, path);
    for (const auto& real : candidates) {
        auto h = std::make_unique<NativeFile>();
        h->ifs = std::make_unique<std::ifstream>(real, std::ios::binary);
        if (h->ifs->is_open()) {
            h->path = real;
            {
                std::lock_guard<std::mutex> lk(pointer->files_mutex);
                pointer->files[file_index] = std::move(h);
            }
            out_handle.value = file_index;
            get_logger()->trace("open_file: {}", real.string());
            return true;
        }
    }
    get_logger()->error("open_file: not found: {}", path);
    return false;
}

static void close_file(FileLoaderHandle loader, FileHandle handle)
{
    if (!handle.valid()) {
        get_logger()->error("close_file: input file handle is invalid!");
        return;
    }

    auto pointer = loader.astype<NativeFSLoader>();

    std::lock_guard<std::mutex> lk(pointer->files_mutex);

    auto it = pointer->files.find(handle.value);
    if (it != pointer->files.end()) {
        pointer->files.erase(it); // RAII closes the streams
    } else {
        get_logger()->error("close_file: invalid file handle {}", handle.value);
    }
}

static bool read_file(FileLoaderHandle loader, FileHandle handle, void* buffer, size_t size, size_t& bytes_read)
{
    bytes_read = 0;

    if (!handle.valid() || !buffer || size == 0) {
        get_logger()->error("read_file: input file handle / buffer / size is invalid!");
        return false;
    }

    auto pointer = loader.astype<NativeFSLoader>();

    std::lock_guard<std::mutex> lk(pointer->files_mutex);

    auto it = pointer->files.find(handle.value);
    if (it == pointer->files.end()) {
        get_logger()->error("read_file: failed to find file by handle {}", handle.value);
        return false;
    }

    auto& h = *it->second;
    if (!h.ifs || !h.ifs->is_open()) {
        get_logger()->error("read_file: file handle {} is not opened for read", handle.value);
        return false;
    }

    // read bytes
    std::istream* in = h.ifs.get();
    in->read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));
    bytes_read = static_cast<size_t>(in->gcount());
    return bytes_read > 0 || in->eof();
}

static bool seek_file(FileLoaderHandle loader, FileHandle handle, int64_t offset)
{
    if (!handle.valid()) {
        get_logger()->error("seek_file: input file handle is invalid!");
        return false;
    }

    auto pointer = loader.astype<NativeFSLoader>();

    auto it = pointer->files.find(handle.value);
    if (it == pointer->files.end()) {
        get_logger()->error("seek_file: failed to find file by handle {}", handle.value);
        return false;
    }

    auto& h = *it->second;
    if (!h.ifs || !h.ifs->is_open()) {
        get_logger()->error("read_file: file handle {} is not opened for read", handle.value);
        return false;
    }

    // seek position
    std::ifstream* io = h.ifs.get();
    io->seekg(offset, std::ios::beg);
    if (io->fail()) return false;
    return true;
}

static bool read_whole_file(FileLoaderHandle loader, FSPath path, void* data)
{
    if (!path || !data) {
        get_logger()->error("read_whole_file: input path or data is invalid!");
        return false;
    }

    auto pointer    = loader.astype<NativeFSLoader>();
    auto candidates = resolve_read_paths(pointer, path);
    for (const auto& real : candidates) {
        std::ifstream in(real, std::ios::binary);
        if (!in.is_open()) continue;
        in.seekg(0, std::ios::end);
        std::streamsize sz = in.tellg();
        in.seekg(0, std::ios::beg);
        if (sz <= 0) continue;
        in.read(static_cast<char*>(data), sz);
        return in.good() || in.eof();
    }
    return false;
}

static bool mount(FileLoaderHandle loader, MountHandle& handle, FSPath vpath, OSPath path, uint priority)
{
    if (!path) {
        get_logger()->error("mount: input path is invalid!");
        return false;
    }

    std::error_code ec;

    auto     vp = normalize_vpath(vpath);
    fs::path root(path);
    root = fs::absolute(root, ec);
    if (ec) {
        get_logger()->error("mount {} -> {}: {}", vp, root.string(), ec.message());
        return false;
    }

    if (!fs::exists(root) || !fs::is_directory(root)) {
        get_logger()->error("mount: root not a directory: {}", root.string());
        return false;
    }

    auto pointer = loader.astype<NativeFSLoader>();

    // assign mount index
    handle.value = pointer->mount_index++;

    // prepare mount point
    NativeMount m;
    m.vpath    = vp;
    m.root     = root;
    m.priority = priority;
    m.mount_id = handle.value;

    // save this mount point, and sort according to priority
    std::lock_guard<std::mutex> lk(pointer->mounts_mutex);
    pointer->mounts.push_back(std::move(m));
    std::sort(pointer->mounts.begin(), pointer->mounts.end(), sort_mount_points);

    get_logger()->info("mount: {} -> {} (prio {})", vp, root.string(), priority);
    return true;
}

static bool unmount(FileLoaderHandle loader, MountHandle handle)
{
    auto pointer = loader.astype<NativeFSLoader>();

    // lock while unmount
    std::lock_guard<std::mutex> lk(pointer->mounts_mutex);

    // remove mount point
    auto before = pointer->mounts.size();
    pointer->mounts.erase(
        std::remove_if(pointer->mounts.begin(), pointer->mounts.end(),
            [&](const NativeMount& m) { return m.mount_id == handle.value; }),
        pointer->mounts.end());
    auto after = pointer->mounts.size();

    get_logger()->info("unmount: id={} (removed {} mounts)", handle.value, (before - after));
    return before != after;
}

// -----------------------------------------------------------------------------
// plugin exports
// -----------------------------------------------------------------------------

LYRA_EXPORT auto prepare() -> void
{
    // do nothing
}

LYRA_EXPORT auto cleanup() -> void
{
    for (auto& loader : g_loaders)
        delete_loader(loader);

    g_loaders.clear();
}

LYRA_EXPORT auto create() -> FileLoaderAPI
{
    auto api            = FileLoaderAPI{};
    api.get_api_name    = get_api_name;
    api.create_loader   = create_loader;
    api.delete_loader   = delete_loader;
    api.sizeof_file     = sizeof_file;
    api.exists_file     = exists_file;
    api.open_file       = open_file;
    api.close_file      = close_file;
    api.read_file       = read_file;
    api.seek_file       = seek_file;
    api.read_whole_file = read_whole_file;
    api.mount           = mount;
    api.unmount         = unmount;
    return api;
}
