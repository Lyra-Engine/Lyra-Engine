#pragma once

#ifndef LYRA_LIBRARY_FILEIO_FS_ENUMS_H
#define LYRA_LIBRARY_FILEIO_FS_ENUMS_H

#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>

namespace lyra
{
    enum struct FSBackend : uint
    {
        NATIVE,
        PHYSFS,
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FILEIO_FS_ENUMS_H
