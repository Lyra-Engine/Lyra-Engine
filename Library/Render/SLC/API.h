#ifndef LYRA_LIBRARY_RENDER_SLC_API_H
#define LYRA_LIBRARY_RENDER_SLC_API_H

#include <Common/Stdint.h>
#include <Common/String.h>
#include <Render/RHI/Descs.h>
#include <Render/SLC/Enums.h>
#include <Render/SLC/Utils.h>
#include <Render/SLC/Descs.h>
#include <Render/SLC/Types.h>

namespace lyra::rhi
{
    struct ShaderAPI
    {
        // backend api name for shader compiler
        CString (*get_api_name)();

        bool (*create_compiler)(CompilerHandle& compiler, const CompilerDescriptor& descriptor);
        void (*delete_compiler)(CompilerHandle compiler);

        // compile from source
        bool (*compile)(CompilerHandle compiler, const CompileDescriptor& desc, CompileResultHandle& result);
        void (*cleanup)(CompileResultHandle result);

        // retrieve shader blobs
        bool (*get_shader_blob)(CompileResultHandle result, CString entry, ShaderBlob& blob);

        // reflect from compiled result
        bool (*reflect)(CompileResultHandle result, GPUPipelineLayoutDescriptor& desc);
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_API_H
