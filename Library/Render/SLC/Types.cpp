#include <Common/Plugin.h>
#include <Render/SLC/API.h>
#include <Render/SLC/Types.h>

using namespace lyra;
using namespace lyra::rhi;

using ShaderPlugin = Plugin<ShaderAPI>;

static Own<ShaderPlugin> SHADER_PLUGIN;

CompileResult::CompileResult(void* res, CString entry)
    : res(res), name(entry) {}

CompileResult::~CompileResult()
{
    SHADER_PLUGIN->get_api()->cleanup(res);
}

auto CompileResult::entry() const -> CString
{
    return name.c_str();
}

auto CompileResult::size() const -> size_t
{
    ShaderBlob  blob;
    ShaderError error;
    return SHADER_PLUGIN->get_api()->compile(res, blob, error);
}

auto CompileResult::code() const -> ShaderBlob
{
    ShaderBlob  blob;
    ShaderError error;
    SHADER_PLUGIN->get_api()->compile(res, blob, error);
    return blob;
}

auto CompileResult::error() const -> ShaderError
{
    ShaderBlob  blob;
    ShaderError error;
    SHADER_PLUGIN->get_api()->compile(res, blob, error);
    return error;
}

void CompileResult::reflect(GPUPipelineLayoutDescriptor& desc) const
{
    SHADER_PLUGIN->get_api()->reflect(res, desc);
}

Compiler Compiler::init(const CompilerDescriptor& descriptor)
{
    if (!SHADER_PLUGIN)
        SHADER_PLUGIN = std::make_unique<ShaderPlugin>("lyra-dxc");

    Compiler compiler;
    compiler.flags  = descriptor.flags;
    compiler.target = descriptor.target;
    return compiler;
}

void Compiler::add_define(const String& macro)
{
    defines.push_back(macro);
}

void Compiler::add_include(const String& path)
{
    includes.push_back(path);
}

auto Compiler::compile(GPUShaderStage stage, const String& entry, const String& source) -> Own<CompileResult>
{
    Vector<const char*> _defines(defines.size(), nullptr);
    std::transform(defines.begin(), defines.end(), _defines.begin(), [](const auto& s) {
        return s.c_str();
    });

    Vector<const char*> _includes(includes.size(), nullptr);
    std::transform(includes.begin(), includes.end(), _includes.begin(), [](const auto& s) {
        return s.c_str();
    });

    CompileDescriptor desc;
    desc.target   = target;
    desc.flags    = flags;
    desc.stage    = stage;
    desc.entry    = entry.c_str();
    desc.source   = source;
    desc.defines  = _defines;
    desc.includes = _includes;

    void* res = nullptr;
    SHADER_PLUGIN->get_api()->prepare(desc, res);
    return std::make_unique<CompileResult>(res, desc.entry);
}
