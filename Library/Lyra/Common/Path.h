#pragma once

#ifndef LYRA_LIBRARY_COMMON_PATH_H
#define LYRA_LIBRARY_COMMON_PATH_H

#include <filesystem>

#include <Lyra/Common/UUID.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/String.h>

namespace lyra
{
    using Path = std::filesystem::path;

    FORCE_INLINE String normalize_path(const String& messy_path)
    {
        std::filesystem::path path(messy_path);
        std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
        return canonicalPath.make_preferred().string();
    }

    FORCE_INLINE std::string extension(const String& path)
    {
        std::filesystem::path fs_path(path);
        return fs_path.extension().string();
    }
} // end of namespace lyra

#endif // LYRA_LIBRARY_COMMON_PATH_H
