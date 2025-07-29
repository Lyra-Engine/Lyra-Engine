#pragma once

#ifndef LYRA_LIBRARY_COMMON_PATH_H
#define LYRA_LIBRARY_COMMON_PATH_H

#include <string>
#include <filesystem>

namespace lyra
{
    using Path = std::filesystem::path;

    inline std::string normalize_path(const std::string& messyPath)
    {
        std::filesystem::path path(messyPath);
        std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
        std::string           npath         = canonicalPath.make_preferred().string();
        return npath;
    }
} // end of namespace lyra

#endif // LYRA_LIBRARY_COMMON_PATH_H
