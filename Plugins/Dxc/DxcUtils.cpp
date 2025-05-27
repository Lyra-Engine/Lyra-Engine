// library headers
#include <Render/SLC/Enums.h>

// local plugin headers
#include "DxcUtils.h"

// disable Window's default min/max macro
#undef min
#undef max

using namespace lyra;

static Logger logger = init_stderr_logger("DXC", LogLevel::trace);

Logger get_logger()
{
    return logger;
}

void DxcArguments::push(CStr arg)
{
    push(to_wstring(arg));
}

void DxcArguments::push(WStr arg)
{
    ptrs.push_back(arg);
}

void DxcArguments::push(WString&& arg)
{
    store.push_back(arg);
    ptrs.push_back(store.back().c_str());
}

#if 0 // TODO: temporarily disabled for compilation
DescriptorType to_descriptor_type(SpvReflectDescriptorType type, bool& error)
{
    switch (type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
            return DescriptorType::Sampler;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return DescriptorType::SampledTexture;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return DescriptorType::StorageTexture;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return DescriptorType::StorageBuffer;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return DescriptorType::UniformBuffer;
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            return DescriptorType::AccelerationStructure;
        default:
            get_logger()->error("Unsupported descriptor type during reflection!");
            error = true;
            return DescriptorType::Invalid;
    }
}
#endif

#ifdef LYRA_WINDOWS
DescriptorType to_descriptor_type(D3D_SHADER_INPUT_TYPE type, bool& error)
{
    switch (type) {
        case D3D_SIT_SAMPLER:
            return DescriptorType::Sampler;
        case D3D_SIT_TEXTURE:
            return DescriptorType::SampledTexture;
        case D3D_SIT_UAV_RWTYPED:
        case D3D_SIT_UAV_RWSTRUCTURED:
            return DescriptorType::StorageTexture;
        case D3D_SIT_UAV_RWBYTEADDRESS:
        case D3D_SIT_UAV_APPEND_STRUCTURED:
            return DescriptorType::StorageBuffer;
        case D3D_SIT_CBUFFER:
            return DescriptorType::UniformBuffer;
        case D3D_SIT_RTACCELERATIONSTRUCTURE:
            return DescriptorType::AccelerationStructure;
        default:
            get_logger()->error("Unsupported descriptor type during reflection!");
            error = true;
            return DescriptorType::Invalid;
    }
}
#endif

#if 0 // TODO: temporarily disabled for compilation
ShaderVisibility to_visibility(ShaderType stage)
{
    switch (stage) {
        case ShaderType::Vertex:
            return ShaderVisibility::Vertex;
        case ShaderType::Fragment:
            return ShaderVisibility::Fragment;
        case ShaderType::Compute:
            return ShaderVisibility::Compute;
        case ShaderType::RayGen:
            return ShaderVisibility::RayGen;
        case ShaderType::Miss:
            return ShaderVisibility::Miss;
        case ShaderType::AnyHit:
            return ShaderVisibility::AnyHit;
        case ShaderType::ClosestHit:
            return ShaderVisibility::ClosestHit;
        case ShaderType::Intersection:
            return ShaderVisibility::Intersection;
    }
    return ShaderVisibility::None;
}
#endif

#if 0 // TODO: temporarily disabled for compilation
Binding* find_binding(ParameterDesc& desc, uint set, uint binding)
{
    Binding* pBinding = nullptr;
    for (auto& bind : desc.bindings) {
        if (bind.set == set && bind.binding == binding) {
            pBinding = &bind;
            break;
        }
    }

    if (pBinding == nullptr) {
        desc.bindings.emplace_back();
        pBinding          = &desc.bindings.back();
        pBinding->set     = set;
        pBinding->binding = binding;
    }

    return pBinding;
}
#endif

void add_target(DxcArguments& args, const CompileDescriptor& desc)
{
    // compile IR target
    switch (desc.target) {
        case CompileTarget::DXIL:
            break;
        case CompileTarget::SPIRV:
            args.push(L"-spirv");
            break;
    }
}

void add_stage(DxcArguments& args, const CompileDescriptor& desc)
{
    // -T for the target profile (eg. 'ps_6_3')
    args.push(L"-T");
    switch (desc.stage) {
        case GPUShaderStage::COMPUTE:
            args.push(L"cs_6_3");
            break;
        case GPUShaderStage::VERTEX:
            args.push(L"vs_6_3");
            break;
        case GPUShaderStage::FRAGMENT:
            args.push(L"ps_6_3");
            break;
        case GPUShaderStage::RAYGEN:
        case GPUShaderStage::MISS:
        case GPUShaderStage::HITGROUP:
        case GPUShaderStage::INTERSECT:
            args.push(L"lib_6_3");
            break;
    }
}

void add_entry(DxcArguments& args, const CompileDescriptor& desc)
{
    // -E for shader entry name
    args.push(L"-E");
    args.push(desc.entry);
}

void add_defines(DxcArguments& args, const CompileDescriptor& desc)
{
    // macros
    for (const auto& define : desc.defines) {
        args.push(L"-D");
        args.push(define);
    }
}

void add_includes(DxcArguments& args, const CompileDescriptor& desc)
{
    // include paths
    for (const auto& include : desc.includes) {
        args.push(L"-I");
        args.push(include);
    }
}

void add_flags(DxcArguments& args, const CompileDescriptor& desc)
{
    bool spirv = desc.target == CompileTarget::SPIRV;

    bool has_debug = desc.flags.contains(CompileFlag::DEBUG);
    if (!has_debug)
        args.push(L"-Qstrip_debug");

    bool has_reflect = desc.flags.contains(CompileFlag::REFLECT);
    if (!has_reflect && !spirv)
        args.push(L"-Qstrip_reflect");
}

void create_reflection(DxcCompiler* pCompiler, DxcCompileResult* pCompiled, const CompileDescriptor& desc)
{
    bool has_reflect = desc.flags.contains(CompileFlag::REFLECT);
    if (!has_reflect) return;

    switch (pCompiled->target) {
        case CompileTarget::DXIL:
            create_dxc_reflection(pCompiler, pCompiled, desc);
            break;
        case CompileTarget::SPIRV:
            create_spv_reflection(pCompiler, pCompiled, desc);
            break;
    }
}

void create_dxc_reflection(DxcCompiler* pCompiler, DxcCompileResult* pCompiled, const CompileDescriptor& desc)
{
#ifdef LYRA_WINDOWS
    HRESULT hr;

    ComPtr<IDxcBlob> reflectionBlob;
    hr = pCompiled->pResult->GetOutput(
        DXC_OUT_REFLECTION,
        __uuidof(IDxcBlob),
        (void**)reflectionBlob.GetAddressOf(),
        nullptr);
    dxc_check(hr, "Failed to get reflection info!");

    DxcBuffer reflectionBuffer;
    reflectionBuffer.Ptr      = reflectionBlob->GetBufferPointer();
    reflectionBuffer.Size     = reflectionBlob->GetBufferSize();
    reflectionBuffer.Encoding = 0;

    hr = pCompiler->pUtils->CreateReflection(
        &reflectionBuffer,
        __uuidof(ID3D12ShaderReflection),
        (void**)pCompiled->pDxcReflect.GetAddressOf());
    dxc_check(hr, "Failed to create reflection!");
#else
    (void)pCompiler;
    (void)pCompiled;
    (void)desc;
    get_logger()->error("DXC reflection is only enabled on Windows!");
#endif
}

void create_spv_reflection(DxcCompiler* pCompiler, DxcCompileResult* pCompiled, const CompileDescriptor& desc)
{
    uint  spirvSize = pCompiled->pBlob->GetBufferSize();
    void* spirvData = pCompiled->pBlob->GetBufferPointer();

    SpvReflectShaderModule module;
    SpvReflectResult       result = spvReflectCreateShaderModule(spirvSize, spirvData, &module);
    spv_check(result, "Error creating shader reflection module");

    pCompiled->pSpvReflect = module;
}

bool reflect_dxc_parameters(DxcCompileResult* pResult, GPUPipelineLayoutDescriptor& desc)
{
    bool error = false;
#ifdef LYRA_WINDOWS
    HRESULT           hr;
    D3D12_SHADER_DESC shaderDesc{};
    hr = pResult->pDxcReflect->GetDesc(&shaderDesc);
    dxc_check(hr, "Failed to reflect shader binding description!");

    for (uint i = 0; i < shaderDesc.BoundResources; i++) {
        D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc{};
        hr = pResult->pDxcReflect->GetResourceBindingDesc(i, &shaderInputBindDesc);
        dxc_check(hr, "Failed to find shader resource binding!");

        uint set     = shaderInputBindDesc.Space;
        uint binding = shaderInputBindDesc.BindPoint;

        Binding* pBinding    = find_binding(desc, set, binding);
        pBinding->binding    = shaderInputBindDesc.BindPoint;
        pBinding->count      = shaderInputBindDesc.BindCount;
        pBinding->type       = to_descriptor_type(shaderInputBindDesc.Type, error);
        pBinding->visibility = pBinding->visibility | to_visibility(pResult->stage);
    }
#else
    (void)pResult;
    (void)desc;
    get_logger()->error("DXC reflection is only enabled on Windows!");
    error = true;
#endif
    return !error;
}

bool reflect_spv_parameters(DxcCompileResult* pResult, GPUPipelineLayoutDescriptor& desc)
{
    bool error = false;

#if 0 // TODO: temporarily disabled for compilation
    const SpvReflectShaderModule& module = pResult->pSpvReflect.value();
    for (uint i = 0; i < module.descriptor_set_count; i++) {
        const SpvReflectDescriptorSet& set = module.descriptor_sets[i];
        for (uint j = 0; j < set.binding_count; j++) {
            const SpvReflectDescriptorBinding& binding = *set.bindings[i];

            // find or create a binding
            Binding* pBinding    = find_binding(desc, binding.set, binding.binding);
            pBinding->binding    = binding.binding;
            pBinding->count      = binding.count;
            pBinding->type       = to_descriptor_type(binding.descriptor_type, error);
            pBinding->visibility = pBinding->visibility | to_visibility(pResult->stage);
        }
    }
#endif
    return !error;
}
