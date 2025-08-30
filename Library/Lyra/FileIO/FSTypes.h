#pragma once

#ifndef LYRA_LIBRARY_FILEIO_FS_TYPES_H
#define LYRA_LIBRARY_FILEIO_FS_TYPES_H

#include <Lyra/Common/Path.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/Common/Container.h>
#include <Lyra/FileIo/FSAPI.h>

namespace lyra
{
    struct FileLoader
    {
    public:
        explicit FileLoader(FSLoader loader);

        bool exists(VFSPath vpath) const;

        auto size(VFSPath vpath) const -> size_t;

        auto open(VFSPath vpath) const -> FileHandle;

        void close(FileHandle file) const;

        auto read(VFSPath vpath) const -> Vector<uint8_t>;

        auto read(FileHandle file, void* buffer, size_t size) const -> size_t;

        void seek(FileHandle file, int64_t offset) const;

        auto mount(VFSPath vpath, FSPath path, uint priority) const -> MountHandle;

        void unmount(MountHandle mount) const;

    private:
        FileLoaderAPI* api = nullptr;
    };

    struct FilePacker
    {
    public:
        explicit FilePacker(FSPacker packer, FSPath path);
        virtual ~FilePacker();

        void write(VFSPath vpath, void* buffer, size_t size) const;

        void write(VFSPath vpath, const Path& path) const;

    private:
        FilePackerAPI* api = nullptr;
        ArchiveHandle  archive;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_FS_TYPES_H
