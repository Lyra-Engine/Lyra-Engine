#include <Lyra/Common/Container.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/FileIO/FSAPI.h>

#include <filesystem>
#include <fstream>
#include <mutex>
#include <memory>
#include <algorithm>

using namespace lyra;

namespace fs = std::filesystem;

static Logger logger = init_stderr_logger("NativeFS", LogLevel::trace);

static inline Logger get_logger()
{
    return logger;
}

// -----------------------------------------------------------------------------
// mount table + path resolution
// -----------------------------------------------------------------------------
struct MountPoint
{
    String   vpath;    // virtual mount prefix, e.g. "/textures" (no trailing slash)
    fs::path root;     // real OS directory root
    uint32_t priority; // higher value = searched earlier
};

static Vector<MountPoint> g_mounts; // guarded by g_mounts_mutex
static std::mutex         g_mounts_mutex;

// normalize virtual path: ensure leading '/', strip trailing '/'
static String normalize_vpath(CString vpath)
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

// resolve a VFS path to a list of real OS paths in search order (read side).
// if the incoming path is absolute on the OS, return it directly.
static Vector<fs::path> resolve_read_paths(CString cpath)
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
    Vector<MountPoint> mounts = g_mounts;
    std::sort(mounts.begin(), mounts.end(), [](const MountPoint& a, const MountPoint& b) {
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

// resolve a VFS path to a single real OS path (write side). We pick the highest
// priority mount whose vpath prefixes the request. If none match and the path is
// absolute, we return it; otherwise we fail with empty path.
static fs::path resolve_write_path(CString cpath)
{
    if (!cpath) return {};

    String path(cpath);
    if (path.empty()) return {};
    std::replace(path.begin(), path.end(), '\\', '/');

    fs::path probe(path);
    if (probe.is_absolute()) return probe;

    std::lock_guard<std::mutex> lock(g_mounts_mutex);
    if (g_mounts.empty()) return {};

    // find the best (highest priority, longest vpath) match
    const MountPoint* best = nullptr;

    size_t best_prefix_len = 0;
    for (const auto& m : g_mounts) {
        size_t plen      = m.vpath.size();
        String path_copy = path;
        if (m.vpath != "/" && !strip_prefix(path_copy, m.vpath)) continue;
        if (!best || m.priority > best->priority ||
            (m.priority == best->priority && plen > best_prefix_len)) {
            best            = &m;
            best_prefix_len = plen;
        }
    }

    if (!best) return {};

    String sub = path;
    if (best->vpath != "/") strip_prefix(sub, best->vpath);

    fs::path real = best->root;
    if (!sub.empty()) real /= fs::path(sub);
    return real;
}

// -----------------------------------------------------------------------------
// File handle bookkeeping
// -----------------------------------------------------------------------------
enum class FileMode
{
    Read,
    Write,
    ReadWrite
};

struct FileHandleData : public FileBase
{
    Own<std::ifstream> ifs;  // used when opened for read
    Own<std::ofstream> ofs;  // used when opened for write
    Own<std::fstream>  iofs; // used when opened with create_file (rw)
    fs::path           path;
    FileMode           mode = FileMode::Read;
};

static std::unordered_map<void*, Own<FileHandleData>> g_handles;
static std::mutex                                     g_handles_mutex;

// -----------------------------------------------------------------------------
// API functions (new interface)
// -----------------------------------------------------------------------------
static CString get_api_name() { return "OSFile"; }

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

static bool create_file(CString path, FileHandle& out_handle, bool overwrite)
{
    out_handle.handle = nullptr;
    if (!path) {
        get_logger()->error("create_file: input path is null!");
        return false;
    }

    try {
        fs::path real = resolve_write_path(path);
        if (real.empty()) {
            get_logger()->error("create_file: failed to resolve write path {}", path);
            return false;
        }

        // ensure parent directories exist
        std::error_code ec;
        fs::create_directories(real.parent_path(), ec);

        std::ios::openmode mode = std::ios::binary | std::ios::out;
        mode |= overwrite ? std::ios::trunc : std::ios::app;

        auto h     = std::make_unique<FileHandleData>();
        h->backend = FSBackend::NATIVE;
        h->iofs    = std::make_unique<std::fstream>(real, mode);
        if (!h->iofs->is_open()) {
            get_logger()->error("create_file: failed to open {}", real.string());
            return false;
        }
        h->path = real;
        h->mode = FileMode::ReadWrite;

        void* key = h.get();
        {
            std::lock_guard<std::mutex> lk(g_handles_mutex);
            g_handles[key] = std::move(h);
        }
        out_handle.handle = key;
        get_logger()->trace("create_file: {}", real.string());
        return true;
    } catch (const std::exception& e) {
        get_logger()->error("create_file: exception: {}", e.what());
        return false;
    }
}

static bool delete_file(CString path)
{
    if (!path) {
        get_logger()->error("delete_file: input path is null!");
        return false;
    }

    fs::path real = resolve_write_path(path);
    if (real.empty()) {
        get_logger()->error("delete_file: failed to resolve write path {}", path);
        return false;
    }

    std::error_code ec;

    bool ok = fs::remove(real, ec);
    if (!ok || ec) {
        get_logger()->error("delete_file: failed {} ({})", real.string(), ec.message());
    }
    return ok && !ec;
}

static bool open_file(CString path, FileHandle& out_handle)
{
    if (!path) {
        get_logger()->error("open_file: input path is null!");
        return false;
    }

    out_handle.handle = nullptr;
    auto candidates   = resolve_read_paths(path);
    for (const auto& real : candidates) {
        auto h     = std::make_unique<FileHandleData>();
        h->backend = FSBackend::NATIVE;
        h->ifs     = std::make_unique<std::ifstream>(real, std::ios::binary);
        if (h->ifs->is_open()) {
            h->path   = real;
            h->mode   = FileMode::Read;
            void* key = h.get();
            {
                std::lock_guard<std::mutex> lk(g_handles_mutex);
                g_handles[key] = std::move(h);
            }
            out_handle.handle = key;
            get_logger()->trace("open_file: {}", real.string());
            return true;
        }
    }
    get_logger()->error("open_file: not found: {}", path);
    return false;
}

static void close_file(FileHandle handle)
{
    if (!handle.handle) {
        get_logger()->error("close_file: input file handle is null!");
        return;
    }

    std::lock_guard<std::mutex> lk(g_handles_mutex);

    auto it = g_handles.find(handle.handle);
    if (it != g_handles.end()) {
        g_handles.erase(it); // RAII closes the streams
    } else {
        get_logger()->error("close_file: invalid handle {}", fmt::ptr(handle.handle));
    }
}

static bool read_file(FileHandle handle, void* buffer, size_t size, size_t& bytes_read)
{
    bytes_read = 0;

    if (!handle.handle || !buffer || size == 0) {
        get_logger()->error("read_file: input file handle / buffer / size is invalid!");
        return false;
    }

    std::lock_guard<std::mutex> lk(g_handles_mutex);

    auto it = g_handles.find(handle.handle);
    if (it == g_handles.end()) {
        get_logger()->error("read_file: failed to find file by handle {}", fmt::ptr(handle.handle));
        return false;
    }

    auto& h = *it->second;
    assert(h.backend == FSBackend::NATIVE);

    std::istream* in = nullptr;
    if (h.ifs && h.ifs->is_open())
        in = h.ifs.get();
    else if (h.iofs && h.iofs->is_open())
        in = h.iofs.get();
    else
        return false;

    in->read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));
    bytes_read = static_cast<size_t>(in->gcount());
    return bytes_read > 0 || in->eof();
}

static bool seek_file(FileHandle handle, int64_t offset)
{
    if (!handle.handle) {
        get_logger()->error("seek_file: input file handle is invalid!");
        return false;
    }

    std::lock_guard<std::mutex> lk(g_handles_mutex);

    auto it = g_handles.find(handle.handle);
    if (it == g_handles.end()) {
        get_logger()->error("seek_file: failed to find file by handle {}", fmt::ptr(handle.handle));
        return false;
    }

    auto& h = *it->second;
    assert(h.backend == FSBackend::NATIVE);

    std::iostream* io = nullptr;
    if (h.iofs && h.iofs->is_open())
        io = h.iofs.get();
    else if (h.ifs && h.ifs->is_open())
        io = reinterpret_cast<std::iostream*>(h.ifs.get());
    else
        return false;

    io->seekg(offset, std::ios::beg);
    if (io->fail()) return false;
    // for bidirectional streams, also move put pointer when possible
    io->seekp(offset, std::ios::beg);
    // ignore put fail for pure ifstream
    return true;
}

static bool read_whole_file(CString path, void* data)
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

static bool write_file(FileHandle handle, const void* buffer, size_t size, size_t& bytes_written)
{
    bytes_written = 0;

    if (!handle.handle || !buffer || size == 0) {
        get_logger()->error("write_file: input file handle / buffer / size is invalid!");
        return false;
    }

    std::lock_guard<std::mutex> lk(g_handles_mutex);

    auto it = g_handles.find(handle.handle);
    if (it == g_handles.end()) {
        get_logger()->error("write_file: failed to find file by handle {}", fmt::ptr(handle.handle));
        return false;
    }

    auto& h = *it->second;
    assert(h.backend == FSBackend::NATIVE);

    std::ostream* out = nullptr;
    if (h.ofs && h.ofs->is_open())
        out = h.ofs.get();
    else if (h.iofs && h.iofs->is_open())
        out = h.iofs.get();
    else
        return false;

    out->write(static_cast<const char*>(buffer), static_cast<std::streamsize>(size));
    if (!out->good()) return false;
    bytes_written = size;
    return true;
}

static bool flush_file(FileHandle handle)
{
    if (!handle.handle) {
        get_logger()->error("flush_file: input file handle is invalid!");
        return false;
    }

    std::lock_guard<std::mutex> lk(g_handles_mutex);

    auto it = g_handles.find(handle.handle);
    if (it == g_handles.end()) {
        get_logger()->error("flush_file: failed to find file by handle {}", fmt::ptr(handle.handle));
        return false;
    }

    auto& h = *it->second;
    assert(h.backend == FSBackend::NATIVE);

    if (h.ofs && h.ofs->is_open()) {
        h.ofs->flush();
        return h.ofs->good();
    }
    if (h.iofs && h.iofs->is_open()) {
        h.iofs->flush();
        return h.iofs->good();
    }
    return false;
}

static bool trunc_file(FileHandle handle, size_t new_size)
{
    if (!handle.handle) {
        get_logger()->error("trunc_file: input file handle is invalid!");
        return false;
    }

    std::lock_guard<std::mutex> lk(g_handles_mutex);

    auto it = g_handles.find(handle.handle);
    if (it == g_handles.end()) {
        get_logger()->error("trunc_file: failed to find file by handle {}", fmt::ptr(handle.handle));
        return false;
    }

    auto& h = *it->second;
    assert(h.backend == FSBackend::NATIVE);

    std::error_code ec;

    // ensure stream buffers are flushed before truncation
    if (it->second->ofs && it->second->ofs->is_open()) it->second->ofs->flush();
    if (it->second->iofs && it->second->iofs->is_open()) it->second->iofs->flush();

    fs::resize_file(h.path, static_cast<uintmax_t>(new_size), ec);
    if (ec) {
        get_logger()->error("trunc_file: failed {} -> {} bytes: {}", h.path.string(), new_size, ec.message());
        return false;
    }
    return true;
}

static bool create_directory(CString path)
{
    if (!path) {
        get_logger()->error("create_directory: input path is invalid!");
        return false;
    }

    fs::path real = resolve_write_path(path);
    if (real.empty()) {
        get_logger()->error("create_directory: failed to resolve real directory: {}", path);
        return false;
    }

    std::error_code ec;

    // create_directories returns false if already exists; treat that as success when directory exists
    bool ok = fs::create_directories(real, ec);
    if (ec) {
        get_logger()->error("create_directory: failed {} : {}", path, ec.message());
        return false;
    }
    return ok || fs::is_directory(real);
}

static bool mount(CString vpath, CString path, uint priority)
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

    MountPoint m;
    m.vpath    = vp;
    m.root     = root;
    m.priority = priority;

    std::lock_guard<std::mutex> lk(g_mounts_mutex);
    g_mounts.push_back(std::move(m));
    get_logger()->info("mount: {} -> {} (prio {})", vp, root.string(), priority);
    return true;
}

static bool unmount(CString vpath)
{
    String vp = normalize_vpath(vpath);

    std::lock_guard<std::mutex> lk(g_mounts_mutex);

    auto before = g_mounts.size();
    g_mounts.erase(std::remove_if(g_mounts.begin(), g_mounts.end(), [&](const MountPoint& m) { return m.vpath == vp; }), g_mounts.end());
    auto after = g_mounts.size();
    get_logger()->info("unmount: {} (removed {} mounts)", vp, (before - after));
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

LYRA_EXPORT auto create() -> FileSystemAPI
{
    auto api             = FileSystemAPI{};
    api.get_api_name     = get_api_name;
    api.sizeof_file      = sizeof_file;
    api.exists_file      = exists_file;
    api.create_file      = create_file;
    api.delete_file      = delete_file;
    api.open_file        = open_file;
    api.close_file       = close_file;
    api.read_file        = read_file;
    api.seek_file        = seek_file;
    api.read_whole_file  = read_whole_file;
    api.write_file       = write_file;
    api.flush_file       = flush_file;
    api.trunc_file       = trunc_file;
    api.create_directory = create_directory;
    api.mount            = mount;
    api.unmount          = unmount;
    return api;
}
