#ifndef LYRA_LIBRARY_RENDER_SLC_API_H
#define LYRA_LIBRARY_RENDER_SLC_API_H

#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/String.h>
#include <Lyra/Render/RHI/Descs.h>
#include <Lyra/Render/SLC/Enums.h>
#include <Lyra/Render/SLC/Utils.h>
#include <Lyra/Render/SLC/Descs.h>
#include <Lyra/Render/SLC/Types.h>

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
