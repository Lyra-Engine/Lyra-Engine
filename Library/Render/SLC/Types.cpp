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

OwnedResource<Compiler> Compiler::init(const CompilerDescriptor& descriptor)
{
    if (!SHADER_PLUGIN)
        SHADER_PLUGIN = std::make_unique<ShaderPlugin>("lyra-slang");

    OwnedResource<Compiler> compiler(new Compiler());
    Compiler::api()->create_compiler(compiler->handle, descriptor);
    return compiler;
}

void Compiler::destroy()
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
    String source = buffer.str();

    auto descriptor   = CompileDescriptor{};
    descriptor.path   = path.u8string().c_str();
    descriptor.module = path.stem().u8string().c_str();
    descriptor.source = source.c_str();
    return compile(descriptor);
}

Own<CompileResult> Compiler::compile(const CompileDescriptor& descriptor)
{
    auto result  = std::make_unique<CompileResult>();
    bool success = Compiler::api()->compile(handle, descriptor, result->handle);
    if (!success)
        throw std::runtime_error("Failed to compile shader!");
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
