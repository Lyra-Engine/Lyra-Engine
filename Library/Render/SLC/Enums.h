#ifndef LYRA_LIBRARY_RENDER_SLC_ENUMS_H
#define LYRA_LIBRARY_RENDER_SLC_ENUMS_H

#include <Common/Stdint.h>

namespace lyra::rhi
{

    enum class CompileFlag : uint
    {
        NONE    = 0x0,
        DEBUG   = 0x1,
        REFLECT = 0x2,
    };

    enum class CompileTarget : uint
    {
        DXIL,
        SPIRV,
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_ENUMS_H
