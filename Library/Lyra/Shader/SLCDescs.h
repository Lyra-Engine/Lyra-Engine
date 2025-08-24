#pragma once

#ifndef LYRA_LIBRARY_SHADER_DESCS_H
#define LYRA_LIBRARY_SHADER_DESCS_H

#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Container.h>
#include <Lyra/Render/RHIEnums.h>
#include <Lyra/Shader/SLCEnums.h>
#include <Lyra/Shader/SLCUtils.h>

namespace lyra
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

} // namespace lyra

#endif // LYRA_LIBRARY_SHADER_DESCS_H
