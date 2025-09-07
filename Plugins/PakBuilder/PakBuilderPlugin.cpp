// system headers
#include <mutex>
#include <memory>
#include <filesystem>
#include <fstream>

// library headers
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/FileIO/FSAPI.h>

// plugin headers
#include "PakBuilderTypes.h"

static Logger logger = init_stderr_logger("PakBuilder", LogLevel::trace);

static HashMap<uint, Own<PakArchiveHandleData>> g_archives;
static std::atomic<uint>                        g_archive_index = 0;
static std::mutex                               g_archives_mutex;

static FORCEINLINE Logger get_logger() { return logger; }

// normalize path for PAK format: convert to forward slashes, no leading slash, max 55 chars
static String normalize_pak_path(FSPath vpath)
{
    if (!vpath) return String("");

    String s(vpath);
    if (s.empty()) return String("");

    // replace backslashes with forward slashes
    std::replace(s.begin(), s.end(), '\\', '/');

    // remove leading '/'
    while (!s.empty() && s.front() == '/')
        s.erase(0, 1);

    // PAK format has a 56-byte filename field (55 chars + null terminator max)
    if (s.length() > 55) {
        get_logger()->warn("normalize_pak_path: truncating path '{}' to 55 characters", s);
        s = s.substr(0, 55);
    }

    return s;
}

// write data in little-endian format
static void write_le32(std::ostream& os, uint32_t value)
{
    uint8_t bytes[4];
    bytes[0] = static_cast<uint8_t>(value & 0xFF);
    bytes[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    bytes[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    bytes[3] = static_cast<uint8_t>((value >> 24) & 0xFF);
    os.write(reinterpret_cast<const char*>(bytes), 4);
}

// finalize and write the PAK file to disk
static bool finalize_pak_archive(PakArchiveHandleData& archive_data)
{
    if (archive_data.is_finalized) {
        return true; // already finalized
    }

    std::ofstream file(archive_data.archive_path, std::ios::binary | std::ios::trunc);
    if (!file) {
        get_logger()->error("finalize_pak_archive: failed to open {} for writing", archive_data.archive_path.string());
        return false;
    }

    // write placeholder header (we'll update it later)
    PakHeader header;
    std::memcpy(header.signature, "PACK", 4);
    header.dir_offset = 0;
    header.dir_size   = 0;
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));

    uint32_t current_offset = sizeof(PakHeader);

    // write file data and track offsets
    using FileInfo = std::pair<uint32_t, uint32_t>; // (offset, size)
    Vector<std::pair<String, FileInfo>> file_info;  // filename -> (offset, size)

    for (const auto& entry : archive_data.files) {
        uint32_t file_offset = current_offset;
        uint32_t file_size   = static_cast<uint32_t>(entry.data.size());

        // write file data
        file.write(reinterpret_cast<const char*>(entry.data.data()), entry.data.size());

        file_info.emplace_back(entry.filename, std::make_pair(file_offset, file_size));
        current_offset += file_size;

        get_logger()->trace("finalize_pak_archive: wrote file '{}' at offset {} (size {})",
            entry.filename, file_offset, file_size);
    }

    // write directory
    uint32_t dir_offset = current_offset;
    uint32_t dir_size   = static_cast<uint32_t>(file_info.size() * sizeof(PakDirEntry));

    for (const auto& info : file_info) {
        PakDirEntry dir_entry;
        std::memset(&dir_entry, 0, sizeof(dir_entry));

        // copy filename (ensure null termination)
        std::strncpy(dir_entry.filename, info.first.c_str(), 55);
        dir_entry.filename[55] = '\0';

        // write offset and size in little-endian
        dir_entry.offset = info.second.first;
        dir_entry.size   = info.second.second;

        // convert to little-endian for cross-platform compatibility
        uint8_t* offset_bytes = reinterpret_cast<uint8_t*>(&dir_entry.offset);
        uint8_t* size_bytes   = reinterpret_cast<uint8_t*>(&dir_entry.size);

        // ensure little-endian byte order
        uint32_t offset_le = info.second.first;
        uint32_t size_le   = info.second.second;

        offset_bytes[0] = static_cast<uint8_t>(offset_le & 0xFF);
        offset_bytes[1] = static_cast<uint8_t>((offset_le >> 8) & 0xFF);
        offset_bytes[2] = static_cast<uint8_t>((offset_le >> 16) & 0xFF);
        offset_bytes[3] = static_cast<uint8_t>((offset_le >> 24) & 0xFF);

        size_bytes[0] = static_cast<uint8_t>(size_le & 0xFF);
        size_bytes[1] = static_cast<uint8_t>((size_le >> 8) & 0xFF);
        size_bytes[2] = static_cast<uint8_t>((size_le >> 16) & 0xFF);
        size_bytes[3] = static_cast<uint8_t>((size_le >> 24) & 0xFF);

        file.write(reinterpret_cast<const char*>(&dir_entry), sizeof(dir_entry));
    }

    // update header with directory info
    file.seekp(0);

    // write signature
    file.write("PACK", 4);

    // write directory offset and size in little-endian
    write_le32(file, dir_offset);
    write_le32(file, dir_size);

    if (!file.good()) {
        get_logger()->error("finalize_pak_archive: failed to write PAK file {}", archive_data.archive_path.string());
        return false;
    }

    archive_data.is_finalized = true;
    get_logger()->info("finalize_pak_archive: successfully wrote PAK file {} with {} files",
        archive_data.archive_path.string(), archive_data.files.size());
    return true;
}

// -----------------------------------------------------------------------------
// API functions
// -----------------------------------------------------------------------------

static CString get_api_name() { return "PakBuilder"; }

static bool open_fn(ArchiveHandle& out_handle, OSPath path)
{
    if (!path) {
        get_logger()->error("open: input path is null!");
        return false;
    }

    std::error_code       ec;
    std::filesystem::path archive_path(path);

    // create parent directories if they don't exist
    std::filesystem::path parent = archive_path.parent_path();
    if (!parent.empty() && !std::filesystem::exists(parent, ec)) {
        if (!std::filesystem::create_directories(parent, ec)) {
            get_logger()->error("open: failed to create parent directories for {}: {}", fmt::ptr(path), ec.message());
            return false;
        }
    }

    auto handle_data          = std::make_unique<PakArchiveHandleData>();
    handle_data->archive_path = archive_path;

    // assign a new archive index
    uint archive_index = g_archive_index++;
    {
        std::lock_guard<std::mutex> lk(g_archives_mutex);
        g_archives[archive_index] = std::move(handle_data);
    }

    out_handle.value = archive_index;
    get_logger()->info("open: opened PAK archive {} with handle {}", fmt::ptr(path), archive_index);
    return true;
}

static void close_fn(ArchiveHandle handle)
{
    if (!handle.valid()) {
        get_logger()->error("close: input archive handle is not valid!");
        return;
    }

    std::lock_guard<std::mutex> lk(g_archives_mutex);

    auto it = g_archives.find(handle.value);
    if (it != g_archives.end()) {
        get_logger()->info("close: finalizing and closing PAK archive with handle {}", handle.value);

        // finalize the archive before closing
        if (!finalize_pak_archive(*it->second)) {
            get_logger()->error("close: failed to finalize PAK archive");
        }

        g_archives.erase(it);
    } else {
        get_logger()->error("close: invalid archive handle {}", handle.value);
    }
}

static bool write_fn(ArchiveHandle handle, FSPath path, void* buffer, size_t size)
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

    String normalized_path = normalize_pak_path(path);
    if (normalized_path.empty()) {
        get_logger()->error("write: normalized path is empty for input path {}", path);
        return false;
    }

    // check if file already exists and replace it
    auto existing = std::find_if(archive_data.files.begin(), archive_data.files.end(),
        [&normalized_path](const PakFileEntry& entry) {
        return entry.filename == normalized_path;
    });

    if (existing != archive_data.files.end()) {
        get_logger()->warn("write: replacing existing file {} in PAK", normalized_path);
        archive_data.files.erase(existing);
    }

    // add new file entry
    PakFileEntry entry;
    entry.filename = normalized_path;
    entry.data.resize(size);
    std::memcpy(entry.data.data(), buffer, size);

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
        if (!finalize_pak_archive(*pair.second))
            get_logger()->error("cleanup: failed to finalize archive with handle {}", pair.first);

    // clear all archives
    g_archives.clear();
}

LYRA_EXPORT auto create() -> FilePackerAPI
{
    auto api         = FilePackerAPI{};
    api.get_api_name = get_api_name;
    api.open         = open_fn;
    api.close        = close_fn;
    api.write        = write_fn;
    return api;
}
