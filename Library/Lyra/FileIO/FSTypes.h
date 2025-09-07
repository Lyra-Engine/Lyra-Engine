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
        explicit FileLoader(FileLoaderAPI* api);
        virtual ~FileLoader();

        bool exists(FSPath vpath) const;

        auto size(FSPath vpath) const -> size_t;

        auto open(FSPath vpath) const -> FileHandle;

        void close(FileHandle file) const;

        template <typename T>
        auto read(FSPath vpath) const -> Vector<T>
        {
            Vector<T> data(size(vpath) / sizeof(T), 0);
            assert(api->read_whole_file(vpath, reinterpret_cast<void*>(data.data())));
            return data;
        }

        auto read(FileHandle file, void* buffer, size_t size) const -> size_t;

        void seek(FileHandle file, int64_t offset) const;

        auto mount(FSPath vpath, OSPath path, uint priority) const -> MountHandle;

        void unmount(MountHandle mount) const;

        FileLoaderAPI* operator()() { return api; }

        FileLoaderAPI* operator()() const { return api; }

    private:
        FileLoaderAPI* api = nullptr;
    };

    struct FilePacker
    {
    public:
        explicit FilePacker(FSPacker packer, OSPath path);
        virtual ~FilePacker();

        void write(FSPath vpath, void* buffer, size_t size) const;

        void write(FSPath vpath, const Path& path) const;

    private:
        FilePackerAPI* api = nullptr;
        ArchiveHandle  archive;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_FS_TYPES_H
