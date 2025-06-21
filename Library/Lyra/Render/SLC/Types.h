#ifndef LYRA_LIBRARY_RENDER_SLC_TYPES_H
#define LYRA_LIBRARY_RENDER_SLC_TYPES_H

#include <Lyra/Common/Path.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/Render/RHI/Enums.h>
#include <Lyra/Render/RHI/Descs.h>
#include <Lyra/Render/SLC/Enums.h>
#include <Lyra/Render/SLC/Utils.h>
#include <Lyra/Render/SLC/Descs.h>

namespace lyra::rhi
{
    struct ShaderAPI;

    struct Compiler;
    struct CompileResult;

    using CompilerHandle      = TypedPointerHandle<Compiler>;
    using CompileResultHandle = TypedPointerHandle<CompileResult>;

    struct CompileResult
    {
        CompileResultHandle handle;

        virtual ~CompileResult();

        void reflect(GPUPipelineLayoutDescriptor& desc) const;

        auto get_shader_blob(CString entry) const -> OwnedShaderBlob;
    };

    struct Compiler
    {
        CompilerHandle handle;

        static auto init(const CompilerDescriptor& descriptor) -> OwnedResource<Compiler>;

        static auto api() -> ShaderAPI*;

        void destroy();

        auto compile(const Path& path) -> Own<CompileResult>;

        auto compile(const CompileDescriptor& descriptor) -> Own<CompileResult>;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_TYPES_H
