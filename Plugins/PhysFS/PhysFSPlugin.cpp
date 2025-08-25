#include <Lyra/Common/Container.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/FileIO/FSAPI.h>

#include <mutex>
#include <memory>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <physfs.h>

using namespace lyra;

namespace fs = std::filesystem;

static Logger logger = init_stderr_logger("PhysFS", LogLevel::trace);

static inline Logger get_logger()
{
    return logger;
}

// custom deleter for PHYSFS_File
struct PhysFSFileDeleter
{
    void operator()(PHYSFS_File* f) const
    {
        if (f) PHYSFS_close(f);
    }
};

using PhysFSFilePtr = std::unique_ptr<PHYSFS_File, PhysFSFileDeleter>;

// -----------------------------------------------------------------------------
// mount table + path resolution
// -----------------------------------------------------------------------------
struct MountPoint
{
    String   vpath;    // virtual mount prefix, e.g. "/textures" (no trailing slash)
    fs::path root;     // real OS directory or archive file
    uint32_t priority; // higher value = searched earlier
};

static Vector<MountPoint> g_mounts; // guarded by g_mounts_mutex
static std::mutex         g_mounts_mutex;

// rebuild the PhysicsFS search path based on current mounts
static void rebuild_mounts()
{
    // Remove all existing entries from the search path
    char** search_path = PHYSFS_getSearchPath();
    if (search_path) {
        for (char** i = search_path; *i; ++i)
            PHYSFS_removeFromSearchPath(*i);
        PHYSFS_freeList(search_path);
    }

    std::lock_guard<std::mutex> lock(g_mounts_mutex);

    // sort mounts by priority descending (higher priority first)
    Vector<MountPoint> sorted = g_mounts;
    std::sort(sorted.begin(), sorted.end(), [](const MountPoint& a, const MountPoint& b) {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.vpath.size() > b.vpath.size(); // longer vpath as tiebreaker
    });

    // mount each in order, appending to the end (higher prio earlier in search order)
    for (const auto& m : sorted) {
        if (!PHYSFS_mount(m.root.string().c_str(), m.vpath.empty() ? nullptr : m.vpath.c_str(), 1)) {
            get_logger()->error("rebuild_mounts: failed to mount {} -> {}: {}", m.vpath, m.root.string(), PHYSFS_getLastError());
        } else {
            get_logger()->trace("rebuild_mounts: mounted {} -> {}", m.vpath, m.root.string());
        }
    }
}

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

// resolve a VFS path to a single real OS path (write side). We pick the highest
// priority mount whose vpath prefixes the request. If none match and the path is
// absolute, we return it; otherwise we fail with empty path.
// additionally, ensure the selected mount root is a directory (writable).
static fs::path resolve_write_path(CString cpath)
{
    if (!cpath) return {};

    String path(cpath);
    if (path.empty()) return {};
    std::replace(path.begin(), path.end(), '\\', '/');

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

    // ensure the mount root is a directory (archives are read-only)
    std::error_code ec;
    if (!fs::is_directory(best->root, ec)) return {};

    String sub = path;
    if (best->vpath != "/") strip_prefix(sub, best->vpath);

    fs::path real = best->root;
    if (!sub.empty()) real /= fs::path(sub);
    return real;
}

// -----------------------------------------------------------------------------
// file handle bookkeeping
// -----------------------------------------------------------------------------
enum class FileMode
{
    Read,
    Write,
    ReadWrite
};

struct FileHandleData : public FileBase
{
    PhysFSFilePtr      pfile; // used when opened via PhysicsFS (read-only)
    Own<std::ifstream> ifs;   // used when opened for read (absolute OS paths)
    Own<std::ofstream> ofs;   // not used (reserved)
    Own<std::fstream>  iofs;  // used when opened with create_file (rw, OS paths)
    fs::path           path;  // real path for OS handles
    FileMode           mode = FileMode::Read;
};

static std::unordered_map<void*, Own<FileHandleData>> g_handles;
static std::mutex                                     g_handles_mutex;

// -----------------------------------------------------------------------------
// API functions
// -----------------------------------------------------------------------------
static CString get_api_name() { return "PAKFile"; }

static size_t sizeof_file(CString path)
{
    if (!path) {
        get_logger()->error("sizeof_file: input path is null!");
        return false;
    }

    String v = normalize_vpath(path);

    PHYSFS_Stat stat;
    if (PHYSFS_stat(v.c_str(), &stat) && stat.filetype == PHYSFS_FILETYPE_REGULAR) {
        get_logger()->trace("sizeof_file: {} -> {} bytes", path, static_cast<unsigned long long>(stat.filesize));
        return static_cast<size_t>(stat.filesize);
    }
    return 0;
}

static bool exists_file(CString path)
{
    if (!path) {
        get_logger()->error("exists_file: input path is null!");
        return false;
    }

    String v = normalize_vpath(path);

    PHYSFS_Stat stat;
    return PHYSFS_stat(v.c_str(), &stat) && stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

static bool create_file(CString, FileHandle&, bool)
{
    get_logger()->error("create_file is not supported by PhysFS!");
    DEBUG_BREAK();
    return false;
}

static bool delete_file(CString)
{
    get_logger()->error("delete_file is not supported by PhysFS!");
    DEBUG_BREAK();
    return false;
}

static bool open_file(CString path, FileHandle& out_handle)
{
    out_handle.handle = nullptr;
    if (!path) {
        get_logger()->error("open_file: input path is null!");
        return false;
    }

    auto h = std::make_unique<FileHandleData>();

    String v = normalize_vpath(path);

    PHYSFS_File* pf = PHYSFS_openRead(v.c_str());
    if (!pf) {
        get_logger()->error("open_file: failed to open {}: {}", v, PHYSFS_getLastError());
        return false;
    }
    h->backend = FSBackend::PHYSFS;
    h->pfile   = PhysFSFilePtr(pf);
    h->mode    = FileMode::Read;

    void* key = h.get();
    {
        std::lock_guard<std::mutex> lk(g_handles_mutex);
        g_handles[key] = std::move(h);
    }
    out_handle.handle = key;
    get_logger()->trace("open_file: {}", path);
    return true;
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
        g_handles.erase(it); // RAII closes the streams/files
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
    if (!h.pfile) return false;

    PHYSFS_sint64 len = PHYSFS_readBytes(h.pfile.get(), buffer, static_cast<PHYSFS_uint64>(size));
    if (len < 0) {
        get_logger()->error("read_file: PhysicsFS error: {}", PHYSFS_getLastError());
        return false;
    }
    bytes_read = static_cast<size_t>(len);
    return true;
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
    if (!h.pfile) return false;

    return PHYSFS_seek(h.pfile.get(), static_cast<PHYSFS_uint64>(offset)) != 0;
}

static bool read_whole_file(CString path, void* data)
{
    if (!path || !data) {
        get_logger()->error("read_whole_file: input path or data is invalid!");
        return false;
    }

    size_t sz = sizeof_file(path);
    if (sz == 0) return false; // not found or empty

    String       v  = normalize_vpath(path);
    PHYSFS_File* pf = PHYSFS_openRead(v.c_str());
    if (!pf) return false;

    PhysFSFilePtr guard(pf); // auto close
    PHYSFS_sint64 len = PHYSFS_readBytes(pf, data, static_cast<PHYSFS_uint64>(sz));
    if (len < 0 || static_cast<size_t>(len) != sz) return false;
    return true;
}

static bool write_file(FileHandle, const void*, size_t, size_t&)
{
    get_logger()->error("write_file is not supported by PhysFS!");
    DEBUG_BREAK();
    return false;
}

static bool flush_file(FileHandle)
{
    get_logger()->error("flush_file is not supported by PhysFS!");
    DEBUG_BREAK();
    return false;
}

static bool trunc_file(FileHandle, size_t)
{
    get_logger()->error("trunc_file is not supported by PhysFS!");
    DEBUG_BREAK();
    return false;
}

static bool create_directory(CString path)
{
    get_logger()->error("create_directory is not supported by PhysFS!");
    DEBUG_BREAK();
    return false;
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

    if (!fs::exists(root, ec)) {
        get_logger()->error("mount: root does not exist: {}", root.string());
        return false;
    }

    MountPoint m;
    m.vpath    = vp;
    m.root     = root;
    m.priority = priority;

    {
        std::lock_guard<std::mutex> lk(g_mounts_mutex);
        g_mounts.push_back(std::move(m));
    }
    rebuild_mounts();
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

    rebuild_mounts();
    get_logger()->info("unmount: {} (removed {} mounts)", vp, (before - after));
    return before != after;
}

// -----------------------------------------------------------------------------
// plugin exports
// -----------------------------------------------------------------------------

LYRA_EXPORT auto prepare() -> void
{
    if (!PHYSFS_init(nullptr)) {
        get_logger()->error("prepare: PHYSFS_init failed: {}", PHYSFS_getLastError());
    }
}

LYRA_EXPORT auto cleanup() -> void
{
    PHYSFS_deinit();
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
