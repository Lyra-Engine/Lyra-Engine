#pragma once

#ifndef LYRA_LIBRARY_FILEIO_FS_UTILS_H
#define LYRA_LIBRARY_FILEIO_FS_UTILS_H

#include <Lyra/Common/Handle.h>
#include <Lyra/Common/String.h>
#include <Lyra/FileIO/FSEnums.h>

namespace lyra
{
    using FSPath  = CString;
    using VFSPath = CString;

    using FileHandle = Handle<FSObjectType, FSObjectType::FILE>;

    using MountHandle = Handle<FSObjectType, FSObjectType::MOUNT>;

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_FS_UTILS_H
