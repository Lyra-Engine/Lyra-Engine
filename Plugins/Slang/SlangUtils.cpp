#include "SlangUtils.h"

static Slang::ComPtr<slang::IGlobalSession> GLOBAL_SESSION;

static Logger logger = init_stderr_logger("Slang", LogLevel::trace);

Logger get_logger()
{
    return logger;
}

void diagnose_if_needed(slang::IBlob* diagnosticsBlob)
{
    if (diagnosticsBlob != nullptr) {
        get_logger()->info("Slang diagnostics: {}", (const char*)diagnosticsBlob->getBufferPointer());
    }
}

void CompilerWrapper::init()
{
    createGlobalSession(GLOBAL_SESSION.writeRef());
}

CompilerWrapper::CompilerWrapper(const CompilerDescriptor& descriptor)
{
    auto target_desc    = slang::TargetDesc{};
    target_desc.format  = select_target(descriptor);
    target_desc.profile = select_profile(descriptor);

    auto session_desc        = slang::SessionDesc{};
    session_desc.targets     = &target_desc;
    session_desc.targetCount = 1;

    Vector<slang::CompilerOptionEntry> options;

    // include dirs
    for (auto& include : descriptor.includes) {
        auto entry               = slang::CompilerOptionEntry{};
        entry.name               = slang::CompilerOptionName::Include;
        entry.value.kind         = slang::CompilerOptionValueKind::String;
        entry.value.stringValue0 = include;
        options.push_back(entry);
    }

    // preprocessor macros
    for (auto& macro : descriptor.defines) {
        auto entry               = slang::CompilerOptionEntry{};
        entry.name               = slang::CompilerOptionName::MacroDefine;
        entry.value.kind         = slang::CompilerOptionValueKind::String;
        entry.value.stringValue0 = macro.first;
        entry.value.stringValue1 = macro.second;
        options.push_back(entry);
    }

    // emit option
    auto emit = slang::CompilerOptionEntry{};
    {
        emit.name            = slang::CompilerOptionName::EmitSpirvDirectly;
        emit.value.kind      = slang::CompilerOptionValueKind::Int;
        emit.value.intValue0 = 1;
        options.push_back(emit);
    }

    GLOBAL_SESSION->createSession(session_desc, session.writeRef());
}

SlangProfileID CompilerWrapper::select_profile(const CompilerDescriptor& descriptor) const
{
    switch (descriptor.target) {
        case CompileTarget::DXIL:
            return GLOBAL_SESSION->findProfile("sm_6_5");
        case CompileTarget::SPIRV:
        default: // fallback for invalid arguments
            return GLOBAL_SESSION->findProfile("spirv_1_5");
    }
}

SlangCompileTarget CompilerWrapper::select_target(const CompilerDescriptor& descriptor) const
{
    switch (descriptor.target) {
        case CompileTarget::DXIL:
            return SLANG_DXIL;
        case CompileTarget::SPIRV:
        default:
            return SLANG_SPIRV;
    }
}

bool CompilerWrapper::compile(const CompileDescriptor& desc, CompileResultInternal& result)
{
    // create shader module
    {
        Slang::ComPtr<slang::IBlob> diagnostics;
        result.module = session->loadModuleFromSourceString(
            desc.module,             // module name
            desc.path,               // module path
            desc.source,             // shader source code
            diagnostics.writeRef()); // optional diagnostic container
        diagnose_if_needed(diagnostics);
    }

    result.session = session;
    return result.module != nullptr;
}

bool CompileResultInternal::get_shader_blob(CString entry, ShaderBlob& blob)
{
    // query entry point
    Slang::ComPtr<slang::IEntryPoint> entry_point;
    {
        Slang::ComPtr<slang::IBlob> diagnostics;
        module->findEntryPointByName(entry, entry_point.writeRef());
        if (!entry_point) {
            get_logger()->info("Failed to get entry point: {}", entry);
            return false;
        }
    }

    // compose modules + entry points
    std::array<slang::IComponentType*, 2> component_types = {module, entry_point};
    Slang::ComPtr<slang::IComponentType>  composed_program;
    {
        Slang::ComPtr<slang::IBlob> diagnostics;
        SlangResult                 result = session->createCompositeComponentType(
            component_types.data(),
            component_types.size(),
            composed_program.writeRef(),
            diagnostics.writeRef());
        diagnose_if_needed(diagnostics);
        SLANG_RETURN_ON_FAIL(result);
    }

    // link
    Slang::ComPtr<slang::IComponentType> linked_program;
    {
        Slang::ComPtr<slang::IBlob> diagnostics;
        SlangResult                 result = composed_program->link(
            linked_program.writeRef(),
            diagnostics.writeRef());
        diagnose_if_needed(diagnostics);
        SLANG_RETURN_ON_FAIL(result);
    }

    // kernel binary
    Slang::ComPtr<slang::IBlob> spirv_code;
    {
        Slang::ComPtr<slang::IBlob> diagnostics;
        SlangResult                 result = linked_program->getEntryPointCode(
            0,
            0,
            spirv_code.writeRef(),
            diagnostics.writeRef());
        diagnose_if_needed(diagnostics);
        SLANG_RETURN_ON_FAIL(result);
    }

    blob.size = static_cast<uint32_t>(spirv_code->getBufferSize());
    blob.data = new uint8_t[blob.size];
    std::memcpy((uint8_t*)blob.data, (uint8_t*)spirv_code->getBufferPointer(), blob.size);
    return true;
}
