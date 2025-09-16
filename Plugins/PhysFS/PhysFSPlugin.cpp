// system headers
#include <mutex>
#include <memory>
#include <algorithm>
#include <filesystem>

// library headers
#include <Lyra/Common/Container.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/FileIO/FSAPI.h>

// plugin headers
#include "PhysFS.h"

static Logger logger = init_stderr_logger("PhysFS", LogLevel::trace);

static inline Logger get_logger() { return logger; }

static Vector<PhysMountPoint>       g_mounts;
static HashMap<uint, Own<PhysFile>> g_files;
static std::atomic<uint>            g_file_index  = 0;
static std::atomic<uint>            g_mount_index = 0;
static std::mutex                   g_mounts_mutex;
static std::mutex                   g_files_mutex;

// rebuild the PhysicsFS search path based on current mounts
static void rebuild_mounts()
{
    // remove all existing entries from the search path
    char** search_path = PHYSFS_getSearchPath();
    if (search_path) {
        for (char** i = search_path; *i; ++i)
            PHYSFS_removeFromSearchPath(*i);
        PHYSFS_freeList(search_path);
    }

    std::lock_guard<std::mutex> lock(g_mounts_mutex);

    // sort mounts by priority descending (higher priority first)
    Vector<PhysMountPoint> sorted = g_mounts;
    std::sort(sorted.begin(), sorted.end(), sort_mount_points);

    // mount each in order, appending to the end (higher prio earlier in search order)
    for (const auto& m : sorted) {
        if (!PHYSFS_mount(m.root.string().c_str(), m.vpath.empty() ? nullptr : m.vpath.c_str(), 1)) {
            get_logger()->error("rebuild_mounts: failed to mount {} -> {}: {}", m.vpath, m.root.string(), PHYSFS_getLastError());
        } else {
            get_logger()->trace("rebuild_mounts: mounted {} -> {}", m.vpath, m.root.string());
        }
    }
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

// -----------------------------------------------------------------------------
// API functions
// -----------------------------------------------------------------------------

static CString get_api_name() { return "PhysFS"; }

static size_t sizeof_file(FSPath path)
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

static bool exists_file(FSPath path)
{
    if (!path) {
        get_logger()->error("exists_file: input path is null!");
        return false;
    }

    String v = normalize_vpath(path);

    PHYSFS_Stat stat;
    return PHYSFS_stat(v.c_str(), &stat) && stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

static bool open_file(FileHandle& out_handle, FSPath path)
{
    if (!path) {
        get_logger()->error("open_file: input path is null!");
        return false;
    }

    auto h = std::make_unique<PhysFile>();
    auto v = normalize_vpath(path);

    PHYSFS_File* pf = PHYSFS_openRead(v.c_str());
    if (!pf) {
        get_logger()->error("open_file: failed to open {}: {}", v, PHYSFS_getLastError());
        return false;
    }
    h->pfile = PhysFSFilePtr(pf);

    // assign a new file index
    uint file_index = g_file_index++;
    {
        std::lock_guard<std::mutex> lk(g_files_mutex);
        g_files[file_index] = std::move(h);
    }

    out_handle.value = file_index;
    get_logger()->trace("open_file: {}", path);
    return true;
}

static void close_file(FileHandle handle)
{
    if (!handle.valid()) {
        get_logger()->error("close_file: input file handle is not valid!");
        return;
    }

    std::lock_guard<std::mutex> lk(g_files_mutex);

    auto it = g_files.find(handle.value);
    if (it != g_files.end()) {
        g_files.erase(it); // RAII closes the streams/files
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
    if (!h.pfile) return false;

    return PHYSFS_seek(h.pfile.get(), static_cast<PHYSFS_uint64>(offset)) != 0;
}

static bool read_whole_file(FSPath path, void* data)
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

static bool mount(MountHandle& out_handle, FSPath vpath, OSPath path, uint priority)
{
    if (!path) {
        get_logger()->error("mount: input path is invalid!");
        return false;
    }

    std::error_code ec;

    auto vp = normalize_vpath(vpath);
    Path root(path);
    root = fs::absolute(root, ec);
    if (ec) {
        get_logger()->error("mount {} -> {}: {}", vp, fmt::ptr(path), ec.message());
        return false;
    }

    if (!fs::exists(root, ec)) {
        get_logger()->error("mount: root does not exist: {}", root.string());
        return false;
    }

    // assign new mount index
    uint mount_id = g_mount_index++;

    PhysMountPoint m;
    m.vpath    = vp;
    m.root     = root;
    m.priority = priority;
    m.mount_id = mount_id;

    // save to global mount points
    {
        std::lock_guard<std::mutex> lk(g_mounts_mutex);
        g_mounts.push_back(std::move(m));
    }
    rebuild_mounts();

    out_handle.value = mount_id;
    get_logger()->info("mount: {} -> {} (prio {})", vp, root.string(), priority);
    return true;
}

static bool unmount(MountHandle handle)
{
    std::lock_guard<std::mutex> lk(g_mounts_mutex);

    auto before = g_mounts.size();
    auto cb     = [&](const PhysMountPoint& m) { return m.mount_id == handle.value; };
    g_mounts.erase(std::remove_if(g_mounts.begin(), g_mounts.end(), cb), g_mounts.end());
    auto after = g_mounts.size();

    rebuild_mounts();
    get_logger()->info("unmount: id={} (removed {} mounts)", handle.value, (before - after));
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
