#pragma once

#ifndef LYRA_LIBRARY_FILEIO_API_H
#define LYRA_LIBRARY_FILEIO_API_H

#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/FileIO/FSEnums.h>
#include <Lyra/FileIO/FSUtils.h>

namespace lyra
{
    struct FileLoaderAPI
    {
        // api name
        CString (*get_api_name)();

        // file query operations
        auto (*sizeof_file)(VFSPath path) -> size_t;
        bool (*exists_file)(VFSPath path);

        // file open/close operations
        bool (*open_file)(FileHandle& handle, FSPath path);
        void (*close_file)(FileHandle handle);

        // file read operations
        bool (*read_file)(FileHandle handle, void* buffer, size_t size, size_t& bytes_read);
        bool (*seek_file)(FileHandle handle, int64_t offset);
        bool (*read_whole_file)(FSPath path, void* data);

        // mount operations
        bool (*mount)(MountHandle& handle, VFSPath vpath, FSPath path, uint priority);
        bool (*unmount)(MountHandle handle);
    };

    struct FilePackerAPI
    {
        // api name
        CString (*get_api_name)();

        // initialize with target archive path
        bool (*open)(ArchiveHandle& handle, FSPath path);
        void (*close)(ArchiveHandle handle);

        // write binary to virtual file system within the archive
        bool (*write)(ArchiveHandle handle, VFSPath path, void* buffer, size_t size);
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_API_H
