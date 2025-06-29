#include "D3D12Utils.h"

void D3D12Frame::init()
{
    auto rhi = get_rhi();

    // init synchronization primitives
    image_available_semaphore = GPUFenceHandle(rhi->fences.add(D3D12Fence(true)));

    // initialize command pools / allocators
    bundle_command_pool.init(D3D12_COMMAND_LIST_TYPE_BUNDLE);
    compute_command_pool.init(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    graphics_command_pool.init(D3D12_COMMAND_LIST_TYPE_DIRECT);
    transfer_command_pool.init(D3D12_COMMAND_LIST_TYPE_COPY);

    // initialize descriptor heap
    descriptor_heap.init(1024, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
}

void D3D12Frame::wait()
{
    if (render_complete_semaphore.valid()) {
        auto rhi = get_rhi();
        fetch_resource(rhi->fences, render_complete_semaphore).wait();
    }
}

void D3D12Frame::reset()
{
    // clean up command pools
    bundle_command_pool.reset();
    compute_command_pool.reset();
    graphics_command_pool.reset();
    transfer_command_pool.reset();

    // clean up descriptor heap
    descriptor_heap.reset();
}

void D3D12Frame::destroy()
{
    reset();

    // destroy command pools
    bundle_command_pool.destroy();
    compute_command_pool.destroy();
    graphics_command_pool.destroy();
    transfer_command_pool.destroy();

    // destroy descriptor heap
    descriptor_heap.destroy();

    auto rhi = get_rhi();

    // destroy synchronization primitives
    fetch_resource(rhi->fences, image_available_semaphore).destroy();
}

GPUCommandEncoderHandle D3D12Frame::allocate(GPUQueueType type, bool primary)
{
    auto rhi = get_rhi();

    D3D12CommandBuffer command_buffer;
    switch (type) {
        case GPUQueueType::TRANSFER:
            command_buffer.command_queue = rhi->transfer_queue;
            if (primary) {
                command_buffer.command_buffer    = transfer_command_pool.allocate();
                command_buffer.command_allocator = transfer_command_pool.command_allocator;
            } else {
                command_buffer.command_buffer    = bundle_command_pool.allocate();
                command_buffer.command_allocator = bundle_command_pool.command_allocator;
            }
            break;
        case GPUQueueType::COMPUTE:
            command_buffer.command_queue = rhi->compute_queue;
            if (primary) {
                command_buffer.command_buffer    = compute_command_pool.allocate();
                command_buffer.command_allocator = compute_command_pool.command_allocator;
            } else {
                command_buffer.command_buffer    = bundle_command_pool.allocate();
                command_buffer.command_allocator = bundle_command_pool.command_allocator;
            }
            break;
        case GPUQueueType::DEFAULT:
        default:
            command_buffer.command_queue = rhi->graphics_queue;
            if (primary) {
                command_buffer.command_buffer    = graphics_command_pool.allocate();
                command_buffer.command_allocator = graphics_command_pool.command_allocator;
            } else {
                command_buffer.command_buffer    = bundle_command_pool.allocate();
                command_buffer.command_allocator = bundle_command_pool.command_allocator;
            }
            break;
    }

    uint handle = allocated_command_buffers.size();
    allocated_command_buffers.push_back(command_buffer);
    return GPUCommandEncoderHandle(handle);
}

GPUBindGroupHandle D3D12Frame::create(const GPUBindGroupDescriptor& desc)
{
    auto rhi        = get_rhi();
    auto layout     = fetch_resource(rhi->bind_group_layouts, desc.layout);
    auto descriptor = layout.create(descriptor_heap, desc);

    uint handle = static_cast<uint>(allocated_descriptors.size());
    allocated_descriptors.push_back(descriptor);
    return GPUBindGroupHandle(handle);
}
