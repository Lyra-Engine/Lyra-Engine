#ifndef LYRA_LIBRARY_RENDER_SLC_DESCS_H
#define LYRA_LIBRARY_RENDER_SLC_DESCS_H

#include <Common/Container.h>
#include <Render/RHI/Enums.h>
#include <Render/SLC/Enums.h>
#include <Render/SLC/Utils.h>

namespace lyra::rhi
{

    struct CompilerDescriptor
    {
        CompileFlags  flags;
        CompileTarget target = CompileTarget::SPIRV;
    };

    struct CompileDescriptor : public CompilerDescriptor
    {
        GPUShaderStage  stage;
        CString         entry;
        String          source;
        Vector<CString> defines;
        Vector<CString> includes;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_DESCS_H
