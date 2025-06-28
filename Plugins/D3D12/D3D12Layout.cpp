// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

D3D12_DESCRIPTOR_RANGE_TYPE infer_buffer_descriptor_type(const GPUBufferBindingLayout& entry)
{
    switch (entry.type) {
        case GPUBufferBindingType::STORAGE:
        case GPUBufferBindingType::READ_ONLY_STORAGE:
            return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        case GPUBufferBindingType::UNIFORM:
            return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        default:
            throw std::invalid_argument("Invalid GPUBufferBindingType!");
    }
}

D3D12_DESCRIPTOR_RANGE_TYPE infer_sampler_descriptor_type(const GPUSamplerBindingLayout&)
{
    return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
}

D3D12_DESCRIPTOR_RANGE_TYPE infer_texture_descriptor_type(const GPUTextureBindingLayout&)
{
    return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
}

D3D12_DESCRIPTOR_RANGE_TYPE infer_storage_texture_descriptor_type(const GPUStorageTextureBindingLayout&)
{
    return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
}

D3D12_DESCRIPTOR_RANGE_TYPE infer_bvh_descriptor_type(const GPUBVHBindingLayout&)
{
    return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
}

D3D12_DESCRIPTOR_RANGE_TYPE infer_descriptor_type(const GPUBindGroupLayoutEntry& entry)
{
    switch (entry.type) {
        case GPUBindingResourceType::BUFFER:
            return infer_buffer_descriptor_type(entry.buffer);
        case GPUBindingResourceType::SAMPLER:
            return infer_sampler_descriptor_type(entry.sampler);
        case GPUBindingResourceType::TEXTURE:
            return infer_texture_descriptor_type(entry.texture);
        case GPUBindingResourceType::STORAGE_TEXTURE:
            return infer_storage_texture_descriptor_type(entry.storage_texture);
        case GPUBindingResourceType::ACCELERATION_STRUCTURE:
            return infer_bvh_descriptor_type(entry.bvh);
        default:
            throw std::invalid_argument("Unsupported GPU binding resource type!");
    }
}

D3D12BindGroupLayout::D3D12BindGroupLayout()
{
    ranges.clear();
    visibility = D3D12_SHADER_VISIBILITY_ALL;
}

D3D12BindGroupLayout::D3D12BindGroupLayout(const GPUBindGroupLayoutDescriptor& desc)
{
    // initialize ranges vector based on descriptor entries
    ranges.reserve(desc.entries.size());

    // determine overall shader visibility
    visibility = D3D12_SHADER_VISIBILITY_ALL;

    // record requires stages
    GPUShaderStageFlags stages = 0;

    for (const auto& entry : desc.entries) {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{});

        auto& range = ranges.back();

        // set binding information
        range.BaseShaderRegister                = entry.binding;
        range.NumDescriptors                    = entry.count;
        range.RegisterSpace                     = 0;
        range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        range.Flags                             = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        range.RangeType                         = infer_descriptor_type(entry);

        // additional special processing
        if (entry.type == GPUBindingResourceType::BUFFER) {
            if (entry.buffer.type == GPUBufferBindingType::READ_ONLY_STORAGE) {
                range.Flags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
            }
        }

        stages = stages | entry.visibility;
    }

    // optimize shader visibility if possible
    bool has_compute  = stages.contains(GPUShaderStage::COMPUTE);
    bool has_vertex   = stages.contains(GPUShaderStage::VERTEX);
    bool has_fragment = stages.contains(GPUShaderStage::FRAGMENT);

    if (has_compute) {
        visibility = D3D12_SHADER_VISIBILITY_ALL; // Compute uses ALL
    } else if (has_vertex && !has_fragment && !has_compute) {
        visibility = D3D12_SHADER_VISIBILITY_VERTEX;
    } else if (has_fragment && !has_vertex && !has_compute) {
        visibility = D3D12_SHADER_VISIBILITY_PIXEL;
    } else {
        visibility = D3D12_SHADER_VISIBILITY_ALL; // Multiple stages or mixed usage
    }
}

void D3D12BindGroupLayout::destroy()
{
    ranges.clear();
    visibility = D3D12_SHADER_VISIBILITY_ALL;
}

D3D12PipelineLayout::D3D12PipelineLayout()
{
    layout = nullptr;
}

D3D12PipelineLayout::D3D12PipelineLayout(const GPUPipelineLayoutDescriptor& desc)
{
    layout = nullptr;

    auto rhi = get_rhi();

    // collect all descriptor ranges from bind group layouts
    Vector<D3D12_ROOT_PARAMETER1> root_parameters;
    for (auto& bind_group_layout : desc.bind_group_layouts) {
        auto& layout = fetch_resource(rhi->bind_group_layouts, bind_group_layout);

        // create root parameter for this bind group's descriptor table
        root_parameters.push_back({});
        auto& root_param                               = root_parameters.back();
        root_param.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        root_param.ShaderVisibility                    = layout.visibility;
        root_param.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(layout.ranges.size());
        root_param.DescriptorTable.pDescriptorRanges   = layout.ranges.data();
    }

    // create root signature descriptor
    D3D12_ROOT_SIGNATURE_DESC1 root_signature_desc = {};
    root_signature_desc.NumParameters              = static_cast<UINT>(root_parameters.size());
    root_signature_desc.pParameters                = root_parameters.data();
    root_signature_desc.NumStaticSamplers          = 0;
    root_signature_desc.pStaticSamplers            = nullptr;
    root_signature_desc.Flags                      = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // use versioned root signature for better performance
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC versioned_desc = {};
    versioned_desc.Version                             = D3D_ROOT_SIGNATURE_VERSION_1_1;
    versioned_desc.Desc_1_1                            = root_signature_desc;

    ID3DBlob* error_blob = nullptr;
    ID3DBlob* signature  = nullptr;
    HRESULT   hr         = D3D12SerializeVersionedRootSignature(&versioned_desc, &signature, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            const char* error_msg = static_cast<const char*>(error_blob->GetBufferPointer());
            get_logger()->error("{}", error_msg);
            error_blob->Release();
            error_blob = nullptr;
        }

        if (signature) {
            signature->Release();
            signature = nullptr;
        }
    }

    if (error_blob)
        error_blob->Release();

    if (FAILED(hr)) return;

    ThrowIfFailed(rhi->device->CreateRootSignature(0, signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&layout)));
}

void D3D12PipelineLayout::destroy()
{
    if (layout) {
        layout->Release();
        layout = nullptr;
    }
}
