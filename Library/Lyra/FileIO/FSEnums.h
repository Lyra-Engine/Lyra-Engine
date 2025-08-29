#pragma once

#ifndef LYRA_LIBRARY_FILEIO_FS_ENUMS_H
#define LYRA_LIBRARY_FILEIO_FS_ENUMS_H

#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>

namespace lyra
{
    enum struct FSLoader : uint
    {
        NATIVE,
        PHYSFS,
    };

    enum struct FSPacker : uint
    {
        PAK,
        ZIP,
    };

    enum struct FSObjectType : uint
    {
        FILE,
        MOUNT,
        ARCHIVE,
    };

    inline constexpr CString to_string(FSObjectType type)
    {
        // clang-format off
        switch (type) {
            case FSObjectType::FILE:    return "FSFile";
            case FSObjectType::MOUNT:   return "FSMount";
            case FSObjectType::ARCHIVE: return "FSArchive";
        }
        // clang-format on
    }

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_FS_ENUMS_H
