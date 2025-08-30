#ifndef LYRA_PLUGIN_ZIP_BUILDER_TYPES_H
#define LYRA_PLUGIN_ZIP_BUILDER_TYPES_H

#include <Lyra/Common/Path.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Container.h>

using namespace lyra;

struct ZipFileEntry
{
    String          filename;
    Vector<uint8_t> data;
};

struct ZipArchiveHandleData
{
    std::filesystem::path archive_path;
    Vector<ZipFileEntry>  files;
    bool                  is_finalized;

    ZipArchiveHandleData() : is_finalized(false) {}
};

#endif // LYRA_PLUGIN_ZIP_BUILDER_TYPES_H
