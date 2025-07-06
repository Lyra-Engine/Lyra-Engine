#ifndef LYRA_PLUGIN_SLANG_UTILS_H
#define LYRA_PLUGIN_SLANG_UTILS_H

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

// both Slang and spdlog includes
#include <Lyra/Common/Logger.h>
#include <Lyra/Render/SLC/API.h>

using namespace lyra;
using namespace lyra::rhi;

auto get_logger() -> Logger;

struct CompileResultInternal
{
    Slang::ComPtr<slang::ISession> session;
    Slang::ComPtr<slang::IModule>  module;

    bool get_shader_blob(CString entry, ShaderBlob& blob);
};

struct CompilerWrapper
{
    Slang::ComPtr<slang::ISession> session;

    static void init();

    explicit CompilerWrapper(const CompilerDescriptor& descriptor);

    auto select_profile(const CompilerDescriptor& descriptor) const -> SlangProfileID;

    auto select_target(const CompilerDescriptor& descriptor) const -> SlangCompileTarget;

    bool compile(const CompileDescriptor& desc, CompileResultInternal& result);
};

#endif // LYRA_PLUGIN_SLANG_UTILS_H
