// reference: https://docs.vulkan.org/guide/latest/hlsl.html

// library headers
#include <Render/SLC/Utils.h>
#include <Render/SLC/Descs.h>

// local plugin headers
#include "DxcUtils.h"
#include "DxcIncludeHandler.h"
#include "spirv_reflect.h"

using namespace lyra;
using namespace lyra::rhi;

bool create_compiler(void*& compiler)
{
    HRESULT hr;

    auto cmp = std::make_unique<DxcCompiler>();

    // utils
    hr = DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), (void**)&cmp->pUtils);
    dxc_check(hr, "Failed to create dxc utils!");

    // compiler
    hr = DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), (void**)&cmp->pCompiler);
    dxc_check(hr, "Failed to create dxc compiler!");

    compiler = cmp.release();
    return true;
}

void delete_compiler(void* compiler)
{
    auto cmp = (DxcCompiler*)compiler;
    delete cmp;
}

bool prepare_compile(void* compiler, const CompileDescriptor& desc, void*& result)
{
    HRESULT hr;

    DxcArguments args;

    auto cmp = (DxcCompiler*)compiler;
    auto res = std::make_unique<DxcCompileResult>();

    // remember the compile target and stage
    // will be used later to determine the way for reflection
    res->target = desc.target;
    res->stage  = desc.stage;

    add_flags(args, desc);
    add_stage(args, desc);
    add_entry(args, desc);
    add_target(args, desc);
    add_defines(args, desc);
    add_includes(args, desc);

    // source code blob
    ComPtr<IDxcBlobEncoding> pSource;
    hr = cmp->pUtils->CreateBlob(
        desc.source.c_str(),
        desc.source.length(),
        CP_UTF8,
        pSource.GetAddressOf());
    dxc_check(hr, "Failed to create dxc blob!");

    // source buffer
    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr      = pSource->GetBufferPointer();
    sourceBuffer.Size     = pSource->GetBufferSize();
    sourceBuffer.Encoding = 0;

    // include handler
    CustomIncludeHandler includeHandler(cmp->pUtils.get());

    // compile result
    hr = cmp->pCompiler->Compile(
        &sourceBuffer,
        args.data(), args.size(),
        &includeHandler,
        __uuidof(IDxcResult), (void**)res->pResult.GetAddressOf());
    dxc_check(hr, "Failed to compile shader!");

    // get the compilation result
    HRESULT compileStatus;
    hr = res->pResult->GetStatus(&compileStatus);
    dxc_check(hr, "Failed to get compile status!");

    // failing to compile
    if (FAILED(compileStatus)) {
        hr = res->pResult->GetErrorBuffer(&res->pErrors);
        if (SUCCEEDED(hr) && res->pErrors) {
            get_logger()->error("Shader compilation failed:\n{}\n",
                (const char*)res->pErrors->GetBufferPointer());
        }
        result = res.release();
        return false;
    }

    // retrieve the compiled blob
    res->pResult->GetResult(&res->pBlob);

    // retrieve the reflection info
    create_reflection(cmp, res.get(), desc);

    result = res.release();
    return true;
}

void cleanup_compile(void* result)
{
    auto res = reinterpret_cast<DxcCompileResult*>(result);
    if (res == nullptr) return;
    if (res->pSpvReflect.has_value())
        spvReflectDestroyShaderModule(&res->pSpvReflect.value());
    delete res;
}

uint compile(void* result, ShaderBlob& blob, ShaderError& errors)
{
    auto res = (DxcCompileResult*)result;

    if (res->pErrors != nullptr) {
        errors = (const char*)res->pErrors->GetBufferPointer();
        blob   = nullptr;
        return 0;
    } else {
        errors = nullptr;
        blob   = (ubyte*)res->pBlob->GetBufferPointer();
        return res->pBlob->GetBufferSize();
    }
}

bool reflect(void* result, GPUPipelineLayoutDescriptor& desc)
{
    DxcCompileResult* res = (DxcCompileResult*)result;
    switch (res->target) {
        case CompileTarget::DXIL:
            return reflect_dxc_parameters(res, desc);
        case CompileTarget::SPIRV:
            return reflect_spv_parameters(res, desc);
    }
    return reflect_spv_parameters(res, desc);
}
