#ifndef LYRA_LIBRARY_RENDER_SLC_UTILS_H
#define LYRA_LIBRARY_RENDER_SLC_UTILS_H

#include <Common/String.h>
#include <Common/BitFlags.h>
#include <Render/SLC/Enums.h>

ENABLE_BIT_FLAGS(lyra::rhi::CompileFlag);

namespace lyra::rhi
{
    using CompileFlags = BitFlags<CompileFlag>;

    using ShaderBlob = uint8_t*;

    using ShaderError = CString;

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_UTILS_H
