#pragma once

#ifndef LYRA_LIBRARY_FILEIO_FS_UTILS_H
#define LYRA_LIBRARY_FILEIO_FS_UTILS_H

#include <Lyra/Common/Path.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/Common/String.h>
#include <Lyra/FileIO/FSEnums.h>

namespace lyra
{
    struct FileLoader;
    struct FilePacker;

    using FileHandle = TypedEnumHandle<FSObjectType, FSObjectType::FILE>;

    using MountHandle = TypedEnumHandle<FSObjectType, FSObjectType::MOUNT>;

    using FileLoaderHandle = TypedPointerHandle<FileLoader>;

    using FilePackerHandle = TypedPointerHandle<FilePacker>;

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_FS_UTILS_H
