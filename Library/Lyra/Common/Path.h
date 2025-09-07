#pragma once

#ifndef LYRA_LIBRARY_COMMON_PATH_H
#define LYRA_LIBRARY_COMMON_PATH_H

#include <filesystem>

#include <Lyra/Common/UUID.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/String.h>

namespace lyra
{
    // C++ path type
    using Path = std::filesystem::path;

    // use OS preferred path type for real file system path
    using OSPath = const std::filesystem::path::value_type*;

    // use const char* for (possibly) virtual file system path
    using FSPath = CString;

} // end of namespace lyra

#endif // LYRA_LIBRARY_COMMON_PATH_H
