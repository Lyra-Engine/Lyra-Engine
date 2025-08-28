// system headers
#include <mutex>
#include <memory>
#include <atomic>
#include <algorithm>

// library headers
#include <Lyra/Common/Container.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/FileIO/FSAPI.h>

// plugin headers
#include "NativeFS.h"

static Logger logger = init_stderr_logger("NativeFS", LogLevel::trace);

static inline Logger get_logger() { return logger; }

static Vector<NativeMountPoint>                 g_mounts;
static HashMap<uint, Own<NativeFileHandleData>> g_files;
static std::atomic<uint>                        g_file_index  = 0;
static std::atomic<uint>                        g_mount_index = 0;
static std::mutex                               g_mounts_mutex;
static std::mutex                               g_files_mutex;

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

// resolve a VFS path to a list of real OS paths in search order (read side).
// if the incoming path is absolute on the OS, return it directly.
static Vector<fs::path> resolve_read_paths(FSPath cpath)
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
    std::lock_guard<std::mutex> lock(g_mounts_mutex);

    // sort mounts by priority (desc) every time only if needed; cheap for small N
    Vector<NativeMountPoint> mounts = g_mounts;
    std::sort(mounts.begin(), mounts.end(), [](const NativeMountPoint& a, const NativeMountPoint& b) {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.vpath.size() > b.vpath.size(); // longer vpath first as tiebreaker
    });

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

static size_t sizeof_file(CString path)
{
    if (!path) {
        get_logger()->error("sizeof_file: input path is null!");
        return false;
    }

    auto candidates = resolve_read_paths(path);
    for (const auto& p : candidates) {
        std::error_code ec;
        if (fs::exists(p, ec) && fs::is_regular_file(p, ec)) {
            auto sz = fs::file_size(p, ec);
            if (!ec) {
                get_logger()->trace("sizeof_file: {} -> {} bytes", path, static_cast<unsigned long long>(sz));
                return true;
            }
        }
    }
    return false;
}

static bool exists_file(CString path)
{
    if (!path) {
        get_logger()->error("exists_file: input path is null!");
        return false;
    }

    auto candidates = resolve_read_paths(path);
    for (const auto& p : candidates) {
        std::error_code ec;
        if (fs::exists(p, ec) && fs::is_regular_file(p, ec))
            return true;
    }
    return false;
}

static bool open_file(FileHandle& out_handle, FSPath path)
{
    if (!path) {
        get_logger()->error("open_file: input path is null!");
        return false;
    }

    // assign a new file index
    uint file_index = g_file_index++;

    // find and open the corresponding file
    auto candidates = resolve_read_paths(path);
    for (const auto& real : candidates) {
        auto h     = std::make_unique<NativeFileHandleData>();
        h->backend = FSBackend::NATIVE;
        h->ifs     = std::make_unique<std::ifstream>(real, std::ios::binary);
        if (h->ifs->is_open()) {
            h->path = real;
            {
                std::lock_guard<std::mutex> lk(g_files_mutex);
                g_files[file_index] = std::move(h);
            }
            out_handle.value = file_index;
            get_logger()->trace("open_file: {}", real.string());
            return true;
        }
    }
    get_logger()->error("open_file: not found: {}", path);
    return false;
}

static void close_file(FileHandle handle)
{
    if (!handle.valid()) {
        get_logger()->error("close_file: input file handle is invalid!");
        return;
    }

    std::lock_guard<std::mutex> lk(g_files_mutex);

    auto it = g_files.find(handle.value);
    if (it != g_files.end()) {
        g_files.erase(it); // RAII closes the streams
    } else {
        get_logger()->error("close_file: invalid file handle {}", handle.value);
    }
}

static bool read_file(FileHandle handle, void* buffer, size_t size, size_t& bytes_read)
{
    bytes_read = 0;

    if (!handle.valid() || !buffer || size == 0) {
        get_logger()->error("read_file: input file handle / buffer / size is invalid!");
        return false;
    }

    std::lock_guard<std::mutex> lk(g_files_mutex);

    auto it = g_files.find(handle.value);
    if (it == g_files.end()) {
        get_logger()->error("read_file: failed to find file by handle {}", handle.value);
        return false;
    }

    auto& h = *it->second;
    if (!h.ifs || h.ifs->is_open()) {
        get_logger()->error("read_file: file handle {} is not opened for read", handle.value);
        return false;
    }

    // read bytes
    std::istream* in = h.ifs;
    in->read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));
    bytes_read = static_cast<size_t>(in->gcount());
    return bytes_read > 0 || in->eof();
}

static bool seek_file(FileHandle handle, int64_t offset)
{
    if (!handle.valid()) {
        get_logger()->error("seek_file: input file handle is invalid!");
        return false;
    }

    std::lock_guard<std::mutex> lk(g_files_mutex);

    auto it = g_files.find(handle.value);
    if (it == g_files.end()) {
        get_logger()->error("seek_file: failed to find file by handle {}", handle.value);
        return false;
    }

    auto& h = *it->second;
    if (!h.ifs || h.ifs->is_open()) {
        get_logger()->error("read_file: file handle {} is not opened for read", handle.value);
        return false;
    }

    // seek position
    std::ifstream* io = h.ifs.get();
    io->seekg(offset, std::ios::beg);
    if (io->fail()) return false;
    return true;
}

static bool read_whole_file(FSPath path, void* data)
{
    if (!path || !data) {
        get_logger()->error("read_whole_file: input path or data is invalid!");
        return false;
    }

    auto candidates = resolve_read_paths(path);
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

static bool mount(MountHandle& handle, VFSPath vpath, FSPath path, uint priority)
{
    if (!path) {
        get_logger()->error("mount: input path is invalid!");
        return false;
    }

    std::error_code ec;

    String   vp = normalize_vpath(vpath);
    fs::path root(path);
    root = fs::absolute(root, ec);
    if (ec) {
        get_logger()->error("mount {} -> {}: {}", vp, path, ec.message());
        return false;
    }

    if (!fs::exists(root) || !fs::is_directory(root)) {
        get_logger()->error("mount: root not a directory: {}", root.string());
        return false;
    }

    // assign mount index
    handle.value = g_mount_index++;

    // prepare mount point
    NativeMountPoint m;
    m.vpath    = vp;
    m.root     = root;
    m.priority = priority;
    m.mount_id = handle.value;

    // save this mount point, and sort according to priority
    std::lock_guard<std::mutex> lk(g_mounts_mutex);
    g_mounts.push_back(std::move(m));
    std::sort(g_mounts.begin(), g_mounts.end(), sort_mount_points);

    get_logger()->info("mount: {} -> {} (prio {})", vp, root.string(), priority);
    return true;
}

static bool unmount(MountHandle handle)
{
    std::lock_guard<std::mutex> lk(g_mounts_mutex);

    auto before = g_mounts.size();
    auto cb     = [&](const NativeMountPoint& m) { return m.mount_id == handle.value; };
    g_mounts.erase(std::remove_if(g_mounts.begin(), g_mounts.end(), cb), g_mounts.end());
    auto after = g_mounts.size();

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
    // do nothing
}

LYRA_EXPORT auto create() -> FileLoaderAPI
{
    auto api            = FileLoaderAPI{};
    api.get_api_name    = get_api_name;
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
