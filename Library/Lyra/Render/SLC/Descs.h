#pragma once

#ifndef LYRA_LIBRARY_RENDER_SLC_DESCS_H
#define LYRA_LIBRARY_RENDER_SLC_DESCS_H

#include <Lyra/Common/Container.h>
#include <Lyra/Render/RHI/Enums.h>
#include <Lyra/Render/SLC/Enums.h>
#include <Lyra/Render/SLC/Utils.h>

namespace lyra::rhi
{

    struct CompilerDescriptor
    {
        CompileFlags   flags    = CompileFlag::NONE;
        CompileTarget  target   = CompileTarget::SPIRV;
        ShaderDefines  defines  = {};
        ShaderIncludes includes = {};
    };

    struct CompileDescriptor
    {
        CString module;
        CString path;
        CString source;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_DESCS_H
