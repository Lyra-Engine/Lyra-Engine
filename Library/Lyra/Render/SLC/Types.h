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
    struct ShaderModule;
    struct ShaderReflection;
    struct ShaderEntryPoint;

    using CompilerHandle         = TypedPointerHandle<Compiler>;
    using ShaderModuleHandle     = TypedPointerHandle<ShaderModule>;
    using ShaderReflectionHandle = TypedPointerHandle<ShaderReflection>;
    using ShaderEntryPoints      = TypedView<ShaderEntryPoint>;

    struct ShaderEntryPoint
    {
        ShaderModuleHandle module;
        CString            entry;
    };

    struct ShaderModule
    {
        ShaderModuleHandle handle;

        virtual ~ShaderModule();

        operator ShaderModuleHandle() { return handle; }
        operator ShaderModuleHandle() const { return handle; }

        auto get_shader_blob(CString entry) const -> OwnedShaderBlob;
    };

    struct ShaderReflection
    {
        ShaderReflectionHandle handle;

        virtual ~ShaderReflection();

        operator ShaderReflectionHandle() { return handle; }
        operator ShaderReflectionHandle() const { return handle; }
    };

    struct Compiler
    {
        CompilerHandle handle;

        static auto init(const CompilerDescriptor& descriptor) -> OwnedResource<Compiler>;

        static auto api() -> ShaderAPI*;

        void destroy();

        auto compile(const Path& path) -> Own<ShaderModule>;

        auto compile(const CompileDescriptor& descriptor) -> Own<ShaderModule>;

        auto reflect(InitList<ShaderEntryPoint> entry_points) -> Own<ShaderReflection>;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_TYPES_H
