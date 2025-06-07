#ifndef LYRA_LIBRARY_RENDER_SLC_TYPES_H
#define LYRA_LIBRARY_RENDER_SLC_TYPES_H

#include <Common/Path.h>
#include <Common/String.h>
#include <Common/Handle.h>
#include <Common/Pointer.h>
#include <Render/RHI/Enums.h>
#include <Render/RHI/Descs.h>
#include <Render/SLC/Enums.h>
#include <Render/SLC/Utils.h>
#include <Render/SLC/Descs.h>

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
