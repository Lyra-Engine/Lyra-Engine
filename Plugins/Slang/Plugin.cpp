// global module headers
#include <Common/String.h>
#include <Common/Plugin.h>
#include <Render/SLC/API.h>

#include "SlangUtils.h"

using namespace lyra;
using namespace lyra::rhi;

auto get_api_name() -> CString { return "Slang"; }

bool create_compiler(CompilerHandle& compiler, const CompilerDescriptor& descriptor)
{
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

bool compile(CompilerHandle compiler, const CompileDescriptor& desc, CompileResultHandle& result)
{
    auto internal = std::make_unique<CompileResultInternal>();
    auto handle   = compiler.astype<CompilerWrapper>();
    handle->compile(desc, *internal);
    result = CompileResultHandle{internal.release()};
    return true;
}

void cleanup(CompileResultHandle result)
{
    if (result.handle) {
        delete reinterpret_cast<CompileResultInternal*>(result.handle);
        result.handle = nullptr;
    }
}

bool get_shader_blob(CompileResultHandle result, CString entry, ShaderBlob& blob)
{
    auto res = reinterpret_cast<CompileResultInternal*>(result.handle);
    res->get_shader_blob(entry, blob);
    return true;
}

bool reflect(CompileResultHandle result, GPUPipelineLayoutDescriptor& desc)
{
    return false;
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
    auto api            = ShaderAPI{};
    api.get_api_name    = get_api_name;
    api.create_compiler = create_compiler;
    api.delete_compiler = delete_compiler;
    api.compile         = compile;
    api.cleanup         = cleanup;
    api.get_shader_blob = get_shader_blob;
    api.reflect         = reflect;
    return api;
}
