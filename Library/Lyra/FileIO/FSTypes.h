#pragma once

#ifndef LYRA_LIBRARY_FILEIO_FS_TYPES_H
#define LYRA_LIBRARY_FILEIO_FS_TYPES_H

#include <Lyra/Common/Plugin.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/FileIo/FSAPI.h>
#include <Lyra/FileIo/FSEnums.h>

namespace lyra
{
    using FilePlugin = Plugin<FileSystemAPI>;

    // File Loader Descritor
    struct FileLoaderDescriptor
    {
        FSBackend backend = FSBackend::OS;
        CString   root    = "";
    };

    struct FileLoader
    {
    public:
        explicit FileLoader();
        virtual ~FileLoader();

        FORCE_INLINE auto api() const -> FilePlugin* { return api_.get(); }

    private:
        Own<FilePlugin> api_;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_FS_TYPES_H
