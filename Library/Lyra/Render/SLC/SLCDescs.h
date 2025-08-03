#pragma once

#ifndef LYRA_LIBRARY_RENDER_SLC_DESCS_H
#define LYRA_LIBRARY_RENDER_SLC_DESCS_H

#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Container.h>
#include <Lyra/Render/RHI/RHIEnums.h>
#include <Lyra/Render/SLC/SLCEnums.h>
#include <Lyra/Render/SLC/SLCUtils.h>

namespace lyra::rhi
{

    struct CompilerDescriptor
    {
        CompileFlags   flags     = CompileFlag::NONE;
        CompileTarget  target    = CompileTarget::SPIRV;
        ShaderDefines  defines   = {};
        ShaderIncludes includes  = {};
        LogLevel       log_level = LogLevel::warn;
    };

    struct CompileDescriptor
    {
        CString module;
        CString path;
        CString source;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_DESCS_H
