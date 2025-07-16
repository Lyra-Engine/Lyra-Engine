#include <fstream>
#include <Lyra/Common/Plugin.h>
#include <Lyra/Render/SLC/API.h>
#include <Lyra/Render/SLC/Types.h>
#include <objidl.h>

using namespace lyra;
using namespace lyra::rhi;

using ShaderPlugin = Plugin<ShaderAPI>;

static Own<ShaderPlugin> SHADER_PLUGIN;

#pragma region Compiler
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

Own<ShaderModule> Compiler::compile(const Path& path)
{
    std::ifstream file(path);
    assert(file.good());

    // read entire file into buffer
    std::stringstream buffer;
    buffer << file.rdbuf();
    String source = buffer.str();

    auto descriptor   = CompileDescriptor{};
    auto path_u8      = path.u8string();
    auto stem_u8      = path.stem().u8string();
    descriptor.path   = path_u8.c_str();
    descriptor.module = stem_u8.c_str();
    descriptor.source = source.c_str();
    return compile(descriptor);
}

Own<ShaderModule> Compiler::compile(const CompileDescriptor& descriptor)
{
    auto result  = std::make_unique<ShaderModule>();
    bool success = Compiler::api()->create_module(handle, descriptor, result->handle);
    if (!success)
        throw std::runtime_error("Failed to compile shader!");
    return result;
}

Own<ShaderReflection> Compiler::reflect(InitList<ShaderEntryPoint> entry_points)
{
    // make a copy of the entry points
    Vector<ShaderEntryPoint> entries = entry_points;

    auto result  = std::make_unique<ShaderReflection>();
    bool success = Compiler::api()->create_reflection(handle, entries, result->handle);
    if (!success)
        throw std::runtime_error("Failed to reflect shader!");
    return result;
}
#pragma endregion Compiler

#pragma region ShaderModule
ShaderModule::~ShaderModule()
{
    Compiler::api()->delete_module(handle);
}

OwnedShaderBlob ShaderModule::get_shader_blob(CString entry) const
{
    OwnedShaderBlob  blob;
    ShaderEntryPoint entry_point{handle, entry};
    blob.reset(new ShaderBlob{});
    Compiler::api()->get_shader_blob(entry_point, *blob.get());
    return blob;
}
#pragma endregion ShaderModule

#pragma region ShaderReflection
ShaderReflection::~ShaderReflection()
{
    Compiler::api()->delete_reflection(handle);
}
#pragma endregion ShaderReflection
