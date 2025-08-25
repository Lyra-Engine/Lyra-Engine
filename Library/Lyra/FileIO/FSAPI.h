#pragma once

#ifndef LYRA_LIBRARY_FILEIO_API_H
#define LYRA_LIBRARY_FILEIO_API_H

#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/FileIO/FSEnums.h>

namespace lyra
{
    // base type for file objects
    // used to sanity check if the file handle is used with correct backend.
    struct FileBase
    {
        FSBackend backend = FSBackend::NATIVE;
    };

    using FileHandle = TypedPointerHandle<FileBase>;

    struct FileSystemAPI
    {
        // api name
        CString (*get_api_name)();

        // file create/delete/sizeof operations
        size_t (*sizeof_file)(CString path);
        bool (*exists_file)(CString path);
        bool (*create_file)(CString path, FileHandle& handle, bool overwrite);
        bool (*delete_file)(CString path); // remove a file from FS

        // file open/close operations
        bool (*open_file)(CString path, FileHandle& handle);
        void (*close_file)(FileHandle handle);

        // file read operations
        bool (*read_file)(FileHandle handle, void* buffer, size_t size, size_t& bytes_read);
        bool (*seek_file)(FileHandle handle, int64_t offset);
        bool (*read_whole_file)(CString path, void* data);

        // file write operations
        bool (*write_file)(FileHandle handle, const void* buffer, size_t size, size_t& bytes_written);
        bool (*flush_file)(FileHandle handle);
        bool (*trunc_file)(FileHandle handle, size_t new_size);

        // directory operations
        bool (*create_directory)(CString path);

        // mount operations
        bool (*mount)(CString vpath, CString path, uint priority);
        bool (*unmount)(CString vpath);
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_API_H
