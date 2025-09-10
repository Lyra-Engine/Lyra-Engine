#ifndef LYRA_PLUGIN_PAK_TYPES_H
#define LYRA_PLUGIN_PAK_TYPES_H

#include <Lyra/Common/Path.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Container.h>

using namespace lyra;

// PAK file format structures (little-endian)
#pragma pack(push, 1)
struct PakHeader
{
    char     signature[4]; // "PACK"
    uint32_t dir_offset;   // offset to directory
    uint32_t dir_size;     // size of directory
};

struct PakDirEntry
{
    char     filename[56]; // null-terminated filename (max 55 chars + null)
    uint32_t offset;       // offset to file data
    uint32_t size;         // size of file data
};
#pragma pack(pop)

struct PakFileEntry
{
    String          filename;
    Vector<uint8_t> data;
};

struct PakArchiveHandleData
{
    std::filesystem::path archive_path;
    Vector<PakFileEntry>  files;
    bool                  is_finalized;

    PakArchiveHandleData() : is_finalized(false) {}
};

#endif // LYRA_PLUGIN_PAK_TYPES_H
