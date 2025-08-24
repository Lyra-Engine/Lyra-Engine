// global module headers
#include <Lyra/Common/String.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/Shader/SLCAPI.h>

#include "SlangUtils.h"

using namespace lyra;

auto get_api_name() -> CString { return "Slang"; }

bool create_compiler(CompilerHandle& compiler, const CompilerDescriptor& descriptor)
{
    // control log level from CompilerDescriptor
    get_logger()->set_level(descriptor.log_level);

    compiler.handle = new CompilerWrapper(descriptor);
    return true;
}

void delete_compiler(CompilerHandle compiler)
{
    if (compiler.handle) {
        delete compiler.astype<CompilerWrapper>();
        compiler.handle = nullptr;
    }
}

bool create_module(CompilerHandle compiler, const CompileDescriptor& desc, ShaderModuleHandle& module)
{
    auto internal = std::make_unique<CompileResultInternal>();
    auto handle   = compiler.astype<CompilerWrapper>();
    if (handle->compile(desc, *internal)) {
        module = ShaderModuleHandle{internal.release()};
        return true;
    }
    return false;
}

void delete_module(ShaderModuleHandle module)
{
    if (module.handle) {
        delete reinterpret_cast<CompileResultInternal*>(module.handle);
        module.handle = nullptr;
    }
}

bool create_reflection(CompilerHandle compiler, ShaderEntryPoints entries, ShaderReflectionHandle& reflection)
{
    auto internal = std::make_unique<ReflectResultInternal>();
    auto handle   = compiler.astype<CompilerWrapper>();
    bool success  = handle->reflect(entries, *internal);

    reflection = ShaderReflectionHandle{internal.release()};
    return success;
}

void delete_reflection(ShaderReflectionHandle reflection)
{
    if (reflection.handle) {
        delete reinterpret_cast<ReflectResultInternal*>(reflection.handle);
        reflection.handle = nullptr;
    }
}

bool get_shader_blob(ShaderEntryPoint entry, ShaderBlob& blob)
{
    auto res = reinterpret_cast<CompileResultInternal*>(entry.module.handle);
    return res->get_shader_blob(entry.entry, blob);
}

bool get_vertex_attributes(ShaderReflectionHandle reflection, ShaderAttributes attrs, GPUVertexAttribute* attributes)
{
    auto res = reinterpret_cast<ReflectResultInternal*>(reflection.handle);
    return res->get_vertex_attributes(attrs, attributes);
}

bool get_push_constant_ranges(ShaderReflectionHandle reflection, uint& count, GPUPushConstantRange* ranges)
{
    auto res = reinterpret_cast<ReflectResultInternal*>(reflection.handle);
    return res->get_push_constant_ranges(count, ranges);
}

bool get_bind_group_layouts(ShaderReflectionHandle reflection, uint& count, GPUBindGroupLayoutDescriptor* layouts)
{
    auto res = reinterpret_cast<ReflectResultInternal*>(reflection.handle);
    return res->get_bind_group_layouts(count, layouts);
}

bool get_bind_group_location(ShaderReflectionHandle handle, CString name, uint& group)
{
    auto res = reinterpret_cast<ReflectResultInternal*>(handle.handle);
    return res->get_bind_group_location(name, group);
}

LYRA_EXPORT auto prepare() -> void
{
    CompilerWrapper::init();
}

LYRA_EXPORT auto cleanup() -> void
{
}

LYRA_EXPORT auto create() -> ShaderAPI
{
    auto api                     = ShaderAPI{};
    api.get_api_name             = get_api_name;
    api.create_compiler          = create_compiler;
    api.delete_compiler          = delete_compiler;
    api.create_module            = create_module;
    api.delete_module            = delete_module;
    api.create_reflection        = create_reflection;
    api.delete_reflection        = delete_reflection;
    api.get_shader_blob          = get_shader_blob;
    api.get_vertex_attributes    = get_vertex_attributes;
    api.get_bind_group_layouts   = get_bind_group_layouts;
    api.get_push_constant_ranges = get_push_constant_ranges;
    return api;
}
