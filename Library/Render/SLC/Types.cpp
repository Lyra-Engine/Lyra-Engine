#include <fstream>

#include <Common/Plugin.h>
#include <Render/SLC/API.h>
#include <Render/SLC/Types.h>

using namespace lyra;
using namespace lyra::rhi;

using ShaderPlugin = Plugin<ShaderAPI>;

static Own<ShaderPlugin> SHADER_PLUGIN;

ShaderAPI* Compiler::api()
{
    return SHADER_PLUGIN->get_api();
}

Compiler Compiler::init(const CompilerDescriptor& descriptor)
{
    if (!SHADER_PLUGIN)
        SHADER_PLUGIN = std::make_unique<ShaderPlugin>("lyra-slang");

    Compiler compiler;
    Compiler::api()->create_compiler(compiler.handle, descriptor);
    return compiler;
}

Compiler::~Compiler()
{
    Compiler::api()->delete_compiler(handle);
}

Own<CompileResult> Compiler::compile(const Path& path)
{
    std::ifstream file(path);
    assert(file.good());

    // read entire file into buffer
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    auto descriptor   = CompileDescriptor{};
    descriptor.path   = path.c_str();
    descriptor.module = path.stem().c_str();
    descriptor.source = source.c_str();
    return compile(descriptor);
}

Own<CompileResult> Compiler::compile(const CompileDescriptor& descriptor)
{
    auto result = std::make_unique<CompileResult>();
    Compiler::api()->compile(handle, descriptor, result->handle);
    return result;
}

CompileResult::~CompileResult()
{
    Compiler::api()->cleanup(handle);
}

OwnedShaderBlob CompileResult::get_shader_blob(CString entry) const
{
    OwnedShaderBlob blob;
    blob.reset(new ShaderBlob{});
    Compiler::api()->get_shader_blob(handle, entry, *blob.get());
    return blob;
}
