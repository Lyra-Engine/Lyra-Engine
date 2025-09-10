// system headers
#include <mutex>
#include <memory>
#include <string>

// library headers
#include <Lyra/Common/Container.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/FileIO/FSAPI.h>

// miniz library header
#include <miniz.h>

// plugin headers
#include "ZipTypes.h"

static Logger logger = init_stderr_logger("Zip", LogLevel::trace);

static HashMap<uint, Own<ZipArchiveHandleData>> g_archives;
static std::atomic<uint>                        g_archive_index = 0;
static std::mutex                               g_archives_mutex;

static FORCEINLINE Logger get_logger() { return logger; }

// normalize virtual path: ensure no leading '/', strip trailing '/'
static String normalize_zip_path(FSPath vpath)
{
    if (!vpath) return String("");

    String s(vpath);
    if (s.empty()) return String("");

    // replace backslashes with forward slashes to be consistent
    std::replace(s.begin(), s.end(), '\\', '/');

    // remove leading '/' for zip entries
    while (!s.empty() && s.front() == '/')
        s.erase(0, 1);

    // remove trailing '/'
    while (s.size() > 1 && s.back() == '/')
        s.pop_back();

    return s;
}

// finalize and write the ZIP file to disk using miniz
static bool finalize_zip_archive(ZipArchiveHandleData& archive_data)
{
    if (archive_data.is_finalized) {
        return true; // already finalized
    }

    // Initialize miniz zip writer
    mz_zip_archive zip_archive;
    std::memset(&zip_archive, 0, sizeof(zip_archive));

    // Create the zip file
    if (!mz_zip_writer_init_file(&zip_archive, archive_data.archive_path.string().c_str(), 0)) {
        get_logger()->error("finalize_zip_archive: failed to initialize zip writer for {}: {}",
            archive_data.archive_path.string(), mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
        return false;
    }

    // Add all files to the archive
    bool success = true;
    for (const auto& entry : archive_data.files) {
        if (!mz_zip_writer_add_mem(&zip_archive, entry.filename.c_str(),
                entry.data.data(), entry.data.size(), MZ_BEST_COMPRESSION)) {
            get_logger()->error("finalize_zip_archive: failed to add file '{}' to archive: {}",
                entry.filename, mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
            success = false;
            break;
        }

        get_logger()->trace("finalize_zip_archive: added file '{}' ({} bytes)",
            entry.filename, entry.data.size());
    }

    // Finalize the archive
    if (success) {
        if (!mz_zip_writer_finalize_archive(&zip_archive)) {
            get_logger()->error("finalize_zip_archive: failed to finalize archive: {}",
                mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
            success = false;
        }
    }

    // Clean up
    if (!mz_zip_writer_end(&zip_archive)) {
        get_logger()->error("finalize_zip_archive: failed to end zip writer: {}",
            mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
        success = false;
    }

    if (success) {
        archive_data.is_finalized = true;
        get_logger()->info("finalize_zip_archive: successfully wrote ZIP file {} with {} files",
            archive_data.archive_path.string(), archive_data.files.size());
    }

    return success;
}

// -----------------------------------------------------------------------------
// API functions
// -----------------------------------------------------------------------------

static CString get_api_name() { return "Zip"; }

static bool open(ArchiveHandle& out_handle, OSPath path)
{
    if (!path) {
        get_logger()->error("open: input path is null!");
        return false;
    }

    std::error_code       ec;
    std::filesystem::path archive_path(path);

    // Create parent directories if they don't exist
    std::filesystem::path parent = archive_path.parent_path();
    if (!parent.empty() && !std::filesystem::exists(parent, ec)) {
        if (!std::filesystem::create_directories(parent, ec)) {
            get_logger()->error("open: failed to create parent directories for {}: {}", fmt::ptr(path), ec.message());
            return false;
        }
    }

    auto handle_data          = std::make_unique<ZipArchiveHandleData>();
    handle_data->archive_path = archive_path;

    // assign a new archive index
    uint archive_index = g_archive_index++;
    {
        std::lock_guard<std::mutex> lk(g_archives_mutex);
        g_archives[archive_index] = std::move(handle_data);
    }

    out_handle.value = archive_index;
    get_logger()->info("open: opened zip archive {} with handle {}", fmt::ptr(path), archive_index);
    return true;
}

static void close(ArchiveHandle handle)
{
    if (!handle.valid()) {
        get_logger()->error("close: input archive handle is not valid!");
        return;
    }

    std::lock_guard<std::mutex> lk(g_archives_mutex);

    auto it = g_archives.find(handle.value);
    if (it != g_archives.end()) {
        get_logger()->info("close: finalizing and closing zip archive with handle {}", handle.value);

        // Finalize the archive before closing
        if (!finalize_zip_archive(*it->second)) {
            get_logger()->error("close: failed to finalize zip archive");
        }

        g_archives.erase(it);
    } else {
        get_logger()->error("close: invalid archive handle {}", handle.value);
    }
}

static bool write(ArchiveHandle handle, FSPath path, void* buffer, size_t size)
{
    if (!handle.valid() || !path || !buffer) {
        get_logger()->error("write: invalid parameters - handle valid: {}, path: {}, buffer: {}",
            handle.valid(), path != nullptr, buffer != nullptr);
        return false;
    }

    std::lock_guard<std::mutex> lk(g_archives_mutex);

    auto it = g_archives.find(handle.value);
    if (it == g_archives.end()) {
        get_logger()->error("write: failed to find archive by handle {}", handle.value);
        return false;
    }

    auto& archive_data = *it->second;
    if (archive_data.is_finalized) {
        get_logger()->error("write: cannot write to finalized archive (handle {})", handle.value);
        return false;
    }

    String normalized_path = normalize_zip_path(path);
    if (normalized_path.empty()) {
        get_logger()->error("write: normalized path is empty for input path {}", path);
        return false;
    }

    // Check if file already exists and replace it
    auto existing = std::find_if(archive_data.files.begin(), archive_data.files.end(),
        [&normalized_path](const ZipFileEntry& entry) {
        return entry.filename == normalized_path;
    });

    if (existing != archive_data.files.end()) {
        get_logger()->warn("write: replacing existing file {} in ZIP", normalized_path);
        archive_data.files.erase(existing);
    }

    // Add new file entry
    ZipFileEntry entry;
    entry.filename = normalized_path;
    entry.data.resize(size);
    if (size > 0) {
        std::memcpy(entry.data.data(), buffer, size);
    }

    archive_data.files.push_back(std::move(entry));

    get_logger()->trace("write: added {} bytes to {} (total files: {})",
        size, normalized_path, archive_data.files.size());
    return true;
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
    if (g_archives.empty()) return;

    std::lock_guard<std::mutex> lk(g_archives_mutex);

    get_logger()->warn("cleanup: {} archives still open, finalizing and closing them", g_archives.size());

    // finalize all open archives
    for (auto& pair : g_archives)
        if (!finalize_zip_archive(*pair.second))
            get_logger()->error("cleanup: failed to finalize archive with handle {}", pair.first);

    // clear all archives
    g_archives.clear();
}

LYRA_EXPORT auto create() -> FilePackerAPI
{
    auto api         = FilePackerAPI{};
    api.get_api_name = get_api_name;
    api.open         = open;
    api.close        = close;
    api.write        = write;
    return api;
}
