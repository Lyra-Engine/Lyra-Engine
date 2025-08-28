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
        auto (*sizeof_file)(FSPath path) -> size_t;
        bool (*exists_file)(FSPath path);

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

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_API_H
