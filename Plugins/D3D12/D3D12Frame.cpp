#include "D3D12Utils.h"

constexpr uint MAX_SAMPLERS_HEAP_SIZE    = 2048u;
constexpr uint MAX_CBV_SRV_UAV_HEAP_SIZE = 1u << 16;

void D3D12Frame::init()
{
    // initialize command pools / allocators
    bundle_command_pool.init(D3D12_COMMAND_LIST_TYPE_BUNDLE);
    compute_command_pool.init(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    graphics_command_pool.init(D3D12_COMMAND_LIST_TYPE_DIRECT);
    transfer_command_pool.init(D3D12_COMMAND_LIST_TYPE_COPY);

    // initialize descriptor heap
    sampler_heap.init(MAX_SAMPLERS_HEAP_SIZE, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    cbv_srv_uav_heap.init(MAX_CBV_SRV_UAV_HEAP_SIZE, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

    D3D12Fence fence(true);
    render_complete_fence = GPUFenceHandle(get_rhi()->fences.add(fence));
}

void D3D12Frame::wait()
{
    if (render_complete_fence.valid()) {
        auto rhi = get_rhi();
        fetch_resource(rhi->fences, render_complete_fence).wait();
    }
}

void D3D12Frame::reset(bool free)
{
    // clean up command pools
    bundle_command_pool.reset();
    compute_command_pool.reset();
    graphics_command_pool.reset();
    transfer_command_pool.reset();

    // clean up descriptor heap
    sampler_heap.reset();
    cbv_srv_uav_heap.reset();

    // reset fence
    fetch_resource(get_rhi()->fences, render_complete_fence).reset();

    if (free) {
        // destroy command buffers
        for (auto& cmd : allocated_command_buffers)
            cmd.cmd.command_buffer->Release();
        allocated_command_buffers.clear();
    } else {
        // reset command buffers
        for (auto& cmd : allocated_command_buffers)
            cmd.reset();
    }
}

void D3D12Frame::destroy()
{
    reset();

    // destroy allocated command buffers
    for (auto& cmd : allocated_command_buffers)
        cmd.cmd.command_buffer->Release();
    allocated_command_buffers.clear();

    // destroy command pools
    bundle_command_pool.destroy();
    compute_command_pool.destroy();
    graphics_command_pool.destroy();
    transfer_command_pool.destroy();

    // destroy descriptor heap
    sampler_heap.destroy();
    cbv_srv_uav_heap.destroy();
}

GPUCommandEncoderHandle D3D12Frame::allocate(GPUQueueType type, bool primary)
{
    auto rhi = get_rhi();

    // set default descriptor heaps
    auto set_descriptor_heap = [&](D3D12CommandBuffer& command_buffer) {
        ID3D12DescriptorHeap* descriptor_heaps[] = {cbv_srv_uav_heap.heap, sampler_heap.heap};
        command_buffer.command_buffer->SetDescriptorHeaps(2, descriptor_heaps);
    };

    // search from existing allocations
    for (uint i = 0; i < (uint)allocated_command_buffers.size(); i++) {
        auto& cmd = allocated_command_buffers.at(i);
        if (!cmd.used && cmd.type == type && cmd.primary == primary) {
            cmd.used = true;
            set_descriptor_heap(cmd.cmd);
            return GPUCommandEncoderHandle(i);
        }
    }

    // new command buffer allocation
    uint handle = allocated_command_buffers.size();
    allocated_command_buffers.push_back(CommandBuffer{});
    CommandBuffer& cmd = allocated_command_buffers.back();
    switch (type) {
        case GPUQueueType::TRANSFER:
            cmd.cmd.command_queue = rhi->transfer_queue;
            if (primary) {
                cmd.cmd.command_buffer    = transfer_command_pool.allocate();
                cmd.cmd.command_allocator = transfer_command_pool.command_allocator;
            } else {
                cmd.cmd.command_buffer    = bundle_command_pool.allocate();
                cmd.cmd.command_allocator = bundle_command_pool.command_allocator;
            }
            break;
        case GPUQueueType::COMPUTE:
            cmd.cmd.command_queue = rhi->compute_queue;
            if (primary) {
                cmd.cmd.command_buffer    = compute_command_pool.allocate();
                cmd.cmd.command_allocator = compute_command_pool.command_allocator;
            } else {
                cmd.cmd.command_buffer    = bundle_command_pool.allocate();
                cmd.cmd.command_allocator = bundle_command_pool.command_allocator;
            }
            break;
        case GPUQueueType::DEFAULT:
        default:
            cmd.cmd.command_queue = rhi->graphics_queue;
            if (primary) {
                cmd.cmd.command_buffer    = graphics_command_pool.allocate();
                cmd.cmd.command_allocator = graphics_command_pool.command_allocator;
            } else {
                cmd.cmd.command_buffer    = bundle_command_pool.allocate();
                cmd.cmd.command_allocator = bundle_command_pool.command_allocator;
            }
            break;
    }

    set_descriptor_heap(cmd.cmd);
    return GPUCommandEncoderHandle(handle);
}

GPUBindGroupHandle D3D12Frame::create(const GPUBindGroupDescriptor& desc)
{
    auto rhi        = get_rhi();
    auto layout     = fetch_resource(rhi->bind_group_layouts, desc.layout);
    auto descriptor = layout.create(*this, desc);

    uint handle = static_cast<uint>(allocated_descriptors.size());
    allocated_descriptors.push_back(descriptor);
    return GPUBindGroupHandle(handle);
}
