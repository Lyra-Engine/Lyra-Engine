#ifndef LYRA_SHADER_DXC_UTILS_H
#define LYRA_SHADER_DXC_UTILS_H

// library headers
#include <Common/Container.h>
#include <Common/Logger.h>
#include <Render/SLC/API.h>

// dxc headers
#ifdef LYRA_WINDOWS
#include <d3d12shader.h>
#endif
#include <dxc/dxcapi.h>

// spv headers
#include <spirv_reflect.h>

// local plugin headers
#include "WinComPtr.h"

using namespace lyra;
using namespace lyra::rhi;

using CStr  = const char*;
using WStr  = const wchar_t*;
using ubyte = uint8_t;

Logger get_logger();

struct DxcCompiler
{
    ComPtr<IDxcUtils>     pUtils;
    ComPtr<IDxcCompiler3> pCompiler;
};

struct DxcCompileResult
{
    CompileTarget            target;
    GPUShaderStage           stage;
    ComPtr<IDxcResult>       pResult;
    ComPtr<IDxcBlob>         pBlob;
    ComPtr<IDxcBlobEncoding> pErrors;
#ifdef LYRA_WINDOWS
    ComPtr<ID3D12ShaderReflection> pDxcReflect;
#endif
    Optional<SpvReflectShaderModule> pSpvReflect = {};
};

struct DxcArguments
{
    Vector<WStr>    ptrs;
    Vector<WString> store;

    void push(CStr arg);
    void push(WStr arg);
    void push(WString&& arg);

    auto data() -> WStr* { return ptrs.data(); }
    auto size() -> size_t { return ptrs.size(); }
};

void add_target(DxcArguments& args, const CompileDescriptor& desc);
void add_stage(DxcArguments& args, const CompileDescriptor& desc);
void add_entry(DxcArguments& args, const CompileDescriptor& desc);
void add_defines(DxcArguments& args, const CompileDescriptor& desc);
void add_includes(DxcArguments& args, const CompileDescriptor& desc);
void add_flags(DxcArguments& args, const CompileDescriptor& desc);

void create_reflection(DxcCompiler* pCompiler, DxcCompileResult* pResult, const CompileDescriptor& desc);
void create_dxc_reflection(DxcCompiler* pCompiler, DxcCompileResult* pResult, const CompileDescriptor& desc);
void create_spv_reflection(DxcCompiler* pCompiler, DxcCompileResult* pResult, const CompileDescriptor& desc);

bool reflect_dxc_parameters(DxcCompileResult* pResult, GPUPipelineLayoutDescriptor& desc);
bool reflect_spv_parameters(DxcCompileResult* pResult, GPUPipelineLayoutDescriptor& desc);

// helper macro to check dxc object creation result
#define dxc_check(result, message)                            \
    {                                                         \
        if (FAILED(result)) {                                 \
            get_logger()->error("{}:{}", __FILE__, __LINE__); \
            get_logger()->error("{}", message);               \
            std::exit(1);                                     \
        }                                                     \
    }

// helper macro to check spv object creation result
#define spv_check(result, message)                            \
    {                                                         \
        if (result != SPV_REFLECT_RESULT_SUCCESS) {           \
            get_logger()->error("{}:{}", __FILE__, __LINE__); \
            get_logger()->error("{}", message);               \
            std::exit(1);                                     \
        }                                                     \
    }

#endif // LYRA_SHADER_DXC_UTILS_H
