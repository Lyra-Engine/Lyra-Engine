#ifndef LYRA_LIBRARY_RENDER_SLC_API_H
#define LYRA_LIBRARY_RENDER_SLC_API_H

#include <Common/Stdint.h>
#include <Common/String.h>
#include <Render/RHI/Descs.h>
#include <Render/SLC/Enums.h>
#include <Render/SLC/Utils.h>
#include <Render/SLC/Descs.h>

namespace lyra::rhi
{
    struct ShaderAPI
    {
        // backend api name for shader compiler
        CString (*get_api_name)();

        // compile from source to SPIRV/DXIL
        bool (*prepare)(const CompileDescriptor& desc, void*& result);
        void (*cleanup)(void* result);

        // get the compile binary
        uint (*compile)(void* result, ShaderBlob& blob, ShaderError& errors);

        // reflect from compiled result
        bool (*reflect)(void* result, GPUPipelineLayoutDescriptor& desc);
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_API_H
