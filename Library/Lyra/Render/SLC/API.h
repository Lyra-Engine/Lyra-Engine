#pragma once

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
        bool (*create_module)(CompilerHandle compiler, const CompileDescriptor& desc, ShaderModuleHandle& module);
        void (*delete_module)(ShaderModuleHandle module);

        // reflect from modules
        bool (*create_reflection)(CompilerHandle compiler, ShaderEntryPoints entries, ShaderReflectionHandle& reflection);
        void (*delete_reflection)(ShaderReflectionHandle reflection);

        // retrieve shader blobs
        bool (*get_shader_blob)(ShaderEntryPoint entry, ShaderBlob& blob);
        bool (*get_vertex_attributes)(ShaderReflectionHandle reflection, ShaderAttributes attrs, GPUVertexAttribute* attributes);
        bool (*get_bind_group_layouts)(ShaderReflectionHandle reflection, uint& count, GPUBindGroupLayoutDescriptor* layouts);
        bool (*get_bind_group_location)(ShaderReflectionHandle reflection, CString name, uint& location);
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_API_H
