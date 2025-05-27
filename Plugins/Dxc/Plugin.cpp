// global module headers
#include <Common/String.h>
#include <Common/Plugin.h>
#include <Render/SLC/API.h>

using namespace lyra;
using namespace lyra::rhi;

static void* COMPILER = nullptr;

auto get_api_name() -> CString { return "DXC"; }

bool create_compiler(void*& compiler);
void delete_compiler(void* compiler);
bool prepare_compile(void* compiler, const CompileDescriptor& desc, void*& result);
void cleanup_compile(void* result);
uint compile(void* result, ShaderBlob& blob, ShaderError& errors);
bool reflect(void* result, GPUPipelineLayoutDescriptor& desc);

bool prepare_compile(const CompileDescriptor& desc, void*& result)
{
    return prepare_compile(COMPILER, desc, result);
}

LYRA_EXPORT auto prepare() -> void
{
    create_compiler(COMPILER);
}

LYRA_EXPORT auto cleanup() -> void
{
    delete_compiler(COMPILER);
}

LYRA_EXPORT auto create() -> ShaderAPI
{
    auto api         = ShaderAPI{};
    api.get_api_name = get_api_name;
    api.prepare      = prepare_compile;
    api.cleanup      = cleanup_compile;
    api.compile      = compile;
    api.reflect      = reflect;
    return api;
}
