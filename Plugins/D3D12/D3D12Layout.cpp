// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"
#include "d3d12.h"
#include <iostream>

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

#pragma region D3D12PipelineLayout
D3D12PipelineLayout::D3D12PipelineLayout()
{
    layout = nullptr;
}

D3D12PipelineLayout::D3D12PipelineLayout(const GPUPipelineLayoutDescriptor& desc)
{
    layout = nullptr;

    auto rhi = get_rhi();

    bind_group_layouts.reserve(desc.bind_group_layouts.size());

    // collect all descriptor ranges from bind group layouts
    Vector<D3D12_ROOT_PARAMETER1> root_parameters;
    root_parameters.reserve(desc.bind_group_layouts.size());
    for (auto& bind_group_layout : desc.bind_group_layouts) {
        auto& layout = fetch_resource(rhi->bind_group_layouts, bind_group_layout);

        // use sequential index for register space
        uint register_space = static_cast<uint>(bind_group_layouts.size());
        for (auto& range : layout.ranges)
            range.RegisterSpace = register_space;

        // record bind group layouts
        bind_group_layouts.push_back(layout);

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

    if (desc.label)
        layout->SetName(to_wstring(desc.label).c_str());
}

void D3D12PipelineLayout::destroy()
{
    if (layout) {
        layout->Release();
        layout = nullptr;
    }
}

void D3D12PipelineLayout::create_dispatch_indirect_signature()
{
    if (signatures.dispatch_indirect) return;

    D3D12_INDIRECT_ARGUMENT_DESC args[1];
    args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

    D3D12_COMMAND_SIGNATURE_DESC program_desc{};
    program_desc.ByteStride       = 12;
    program_desc.NumArgumentDescs = 1;
    program_desc.pArgumentDescs   = args;

    auto rhi = get_rhi();
    ThrowIfFailed(rhi->device->CreateCommandSignature(&program_desc, layout, IID_PPV_ARGS(&signatures.dispatch_indirect)));
}

void D3D12PipelineLayout::create_draw_indirect_signature()
{
    if (signatures.draw_indirect) return;

    D3D12_INDIRECT_ARGUMENT_DESC args[1];
    args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

    D3D12_COMMAND_SIGNATURE_DESC program_desc{};
    program_desc.ByteStride       = 16;
    program_desc.NumArgumentDescs = 1;
    program_desc.pArgumentDescs   = args;

    auto rhi = get_rhi();
    ThrowIfFailed(rhi->device->CreateCommandSignature(&program_desc, layout, IID_PPV_ARGS(&signatures.draw_indirect)));
}

void D3D12PipelineLayout::create_draw_indexed_indirect_signature()
{
    if (signatures.draw_indexed_indirect) return;

    D3D12_INDIRECT_ARGUMENT_DESC args[1];
    args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

    D3D12_COMMAND_SIGNATURE_DESC program_desc{};
    program_desc.ByteStride       = 20;
    program_desc.NumArgumentDescs = 1;
    program_desc.pArgumentDescs   = args;

    auto rhi = get_rhi();
    ThrowIfFailed(rhi->device->CreateCommandSignature(&program_desc, layout, IID_PPV_ARGS(&signatures.draw_indexed_indirect)));
}

#pragma endregion D3D12PipelineLayout

#pragma region D3D12BindGroupLayout
D3D12BindGroupLayout::D3D12BindGroupLayout()
{
    ranges.clear();
    visibility = D3D12_SHADER_VISIBILITY_ALL;
}

D3D12BindGroupLayout::D3D12BindGroupLayout(const GPUBindGroupLayoutDescriptor& desc)
{
    bindless = desc.bindless;

    // initialize ranges vector based on descriptor entries
    ranges.reserve(desc.entries.size());

    // determine overall shader visibility
    visibility = D3D12_SHADER_VISIBILITY_ALL;

    // record requires stages
    GPUShaderStageFlags stages = 0;

    // binding info accumulators
    uint sampler_base_offset     = 0;
    uint cbv_srv_uav_base_offset = 0;

    // check binding type (D3D12 has requirement that sampler cannot be mixed with others)
    bool has_sampler     = false;
    bool has_cbv_srv_uav = false;
    for (const auto& entry : desc.entries) {
        if (entry.type == GPUBindingResourceType::SAMPLER)
            has_sampler = true;
        else
            has_cbv_srv_uav = true;
    }
    if (has_sampler)
        assert(!has_cbv_srv_uav && "sampler and cbv_srv_uav bindings cannot be in the same register space!");
    if (has_cbv_srv_uav)
        assert(!has_sampler && "sampler and cbv_srv_uav bindings cannot be in the same register space!");

    for (const auto& entry : desc.entries) {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{});

        // set binding information
        auto& range                             = ranges.back();
        range.BaseShaderRegister                = entry.binding;
        range.NumDescriptors                    = entry.count;
        range.RegisterSpace                     = 0; // NOTE: need to be changed later in the pipeline layout
        range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        range.Flags                             = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
        range.RangeType                         = infer_descriptor_type(entry);

        // additional special processing
        if (entry.type == GPUBindingResourceType::BUFFER)
            if (entry.buffer.type == GPUBufferBindingType::READ_ONLY_STORAGE)
                range.Flags |= D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;

        // prepare bind info
        bindings.push_back(D3D12BindInfo{});
        auto& binding         = bindings.back();
        binding.binding_index = entry.binding;
        binding.binding_count = entry.count;
        if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER) {
            binding.base_offset = sampler_base_offset;
            sampler_base_offset += range.NumDescriptors;
        } else {
            binding.base_offset = cbv_srv_uav_base_offset;
            cbv_srv_uav_base_offset += range.NumDescriptors;
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

D3D12BindGroup D3D12BindGroupLayout::create(D3D12Frame& frame, const GPUBindGroupDescriptor& desc)
{
    assert(!bindless && "Cannot create bindless descriptor using bound descriptor entries!");

    bool use_sampler_heap = false;
    for (auto& entry : desc.entries)
        if (entry.type == GPUBindingResourceType::SAMPLER)
            use_sampler_heap = true;

    // find out descriptor count
    uint count = false;
    for (auto& entry : bindings)
        count += entry.binding_count;

    // allocate descriptors
    D3D12BindGroup bind_group;
    if (use_sampler_heap)
        bind_group.descriptor = frame.sampler_heap.allocate(count);
    else
        bind_group.descriptor = frame.cbv_srv_uav_heap.allocate(count);

    // write to descriptors
    for (auto& entry : desc.entries) {
        uint offset = bindings.at(entry.binding).base_offset;
        copy_regular_descriptors(frame, entry, bind_group, offset);
    }

    return bind_group;
}

void D3D12BindGroupLayout::copy_regular_descriptors(const D3D12Frame& frame, const GPUBindGroupEntry& entry, D3D12BindGroup& bind_group, uint offset)
{
    switch (entry.type) {
        case GPUBindingResourceType::SAMPLER:
            copy_sampler_descriptor(frame, entry, bind_group, offset);
            break;
        case GPUBindingResourceType::BUFFER:
            create_buffer_descriptor(frame, entry, bind_group, offset);
            break;
        case GPUBindingResourceType::TEXTURE:
        case GPUBindingResourceType::STORAGE_TEXTURE:
            copy_texture_descriptor(frame, entry, bind_group, offset);
            break;
        case GPUBindingResourceType::ACCELERATION_STRUCTURE:
            break;
        default:
            assert(!!!"Invaid GPUBindingResourceType");
    }
}

void D3D12BindGroupLayout::copy_sampler_descriptor(const D3D12Frame& frame, const GPUBindGroupEntry& entry, D3D12BindGroup& bind_group, uint offset)
{
    auto  rhi = get_rhi();
    auto& smp = fetch_resource(rhi->samplers, entry.sampler);

    D3D12_CPU_DESCRIPTOR_HANDLE src_handle = smp.sampler.handle;
    D3D12_CPU_DESCRIPTOR_HANDLE dst_handle = bind_group.descriptor.cpu_handle;
    dst_handle.ptr += (offset + entry.index) * frame.sampler_heap.increment;
    rhi->device->CopyDescriptorsSimple(1, dst_handle, src_handle, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

void D3D12BindGroupLayout::copy_texture_descriptor(const D3D12Frame& frame, const GPUBindGroupEntry& entry, D3D12BindGroup& bind_group, uint offset)
{
    auto  rhi = get_rhi();
    auto& tex = fetch_resource(rhi->views, entry.texture);

    D3D12_CPU_DESCRIPTOR_HANDLE src_handle = entry.type == GPUBindingResourceType::TEXTURE ? tex.srv_view.handle : tex.uav_view.handle;
    D3D12_CPU_DESCRIPTOR_HANDLE dst_handle = bind_group.descriptor.cpu_handle;
    dst_handle.ptr += (offset + entry.index) * frame.cbv_srv_uav_heap.increment;
    rhi->device->CopyDescriptorsSimple(1, dst_handle, src_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3D12BindGroupLayout::create_buffer_descriptor(const D3D12Frame& frame, const GPUBindGroupEntry& entry, D3D12BindGroup& bind_group, uint offset)
{
    auto type = ranges.at(entry.binding).RangeType;
    switch (type) {
        case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
            create_buffer_cbv_descriptor(frame, entry, bind_group, offset);
            break;
        case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
            create_buffer_uav_descriptor(frame, entry, bind_group, offset);
            break;
        default:
            assert(!!!"Buffer descriptor only supports CBV and UAV!");
    }
}

void D3D12BindGroupLayout::create_buffer_cbv_descriptor(const D3D12Frame& frame, const GPUBindGroupEntry& entry, D3D12BindGroup& bind_group, uint offset)
{
    auto  rhi = get_rhi();
    auto& buf = fetch_resource(rhi->buffers, entry.buffer.buffer);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
    cbv_desc.BufferLocation = buf.buffer->GetGPUVirtualAddress() + entry.buffer.offset;
    cbv_desc.SizeInBytes    = entry.buffer.size == 0 ? buf.size() : entry.buffer.size;
    cbv_desc.SizeInBytes    = (cbv_desc.SizeInBytes + 255) & ~255; // CBV alignment

    auto descriptor = bind_group.descriptor;
    descriptor.cpu_handle.ptr += (offset + entry.index) * frame.cbv_srv_uav_heap.increment;
    rhi->device->CreateConstantBufferView(&cbv_desc, descriptor.cpu_handle);
}

void D3D12BindGroupLayout::create_buffer_uav_descriptor(const D3D12Frame& frame, const GPUBindGroupEntry& entry, D3D12BindGroup& bind_group, uint offset)
{
    auto  rhi = get_rhi();
    auto& buf = fetch_resource(rhi->buffers, entry.buffer.buffer);

    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
    uav_desc.Format                      = DXGI_FORMAT_R8_UINT;
    uav_desc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
    uav_desc.Buffer.FirstElement         = entry.buffer.offset;
    uav_desc.Buffer.NumElements          = entry.buffer.size == 0 ? buf.size() : entry.buffer.size;
    uav_desc.Buffer.StructureByteStride  = 1;
    uav_desc.Buffer.CounterOffsetInBytes = 0;
    uav_desc.Buffer.Flags                = D3D12_BUFFER_UAV_FLAG_NONE;

    auto descriptor = bind_group.descriptor;
    descriptor.cpu_handle.ptr += (offset + entry.index) * frame.cbv_srv_uav_heap.increment;
    rhi->device->CreateUnorderedAccessView(buf.buffer, nullptr, &uav_desc, descriptor.cpu_handle);
}
#pragma endregion D3D12BindGroupLayout
