#include "VkUtils.h"

void VulkanFrame::init()
{
    inflight_fence = create_vkfence(true);

    create_fence(image_available_semaphore, VK_SEMAPHORE_TYPE_BINARY);
    create_fence(render_complete_semaphore, VK_SEMAPHORE_TYPE_BINARY);

    auto rhi = get_rhi();

    if (rhi->queues.compute.has_value())
        compute_command_pool = create_command_pool(rhi->queues.compute.value());

    if (rhi->queues.graphics.has_value())
        graphics_command_pool = create_command_pool(rhi->queues.graphics.value());

    if (rhi->queues.transfer.has_value())
        transfer_command_pool = create_command_pool(rhi->queues.transfer.value());
}

void VulkanFrame::wait()
{
    wait_vkfence(inflight_fence);
}

void VulkanFrame::reset()
{
    reset_vkfence(inflight_fence);

    if (compute_command_pool != VK_NULL_HANDLE)
        reset_command_pool(compute_command_pool);

    if (graphics_command_pool != VK_NULL_HANDLE)
        reset_command_pool(graphics_command_pool);

    if (transfer_command_pool != VK_NULL_HANDLE)
        reset_command_pool(transfer_command_pool);

    allocated_command_buffers.clear();
}

GPUCommandEncoderHandle VulkanFrame::allocate(GPUQueueType type, bool primary)
{
    auto rhi = get_rhi();

    VulkanCommandBuffer command_buffer;
    command_buffer.frame_id = frame_id;
    switch (type) {
        case GPUQueueType::DEFAULT:
            command_buffer.command_queue  = rhi->graphics_queue;
            command_buffer.command_buffer = allocate_command_buffer(graphics_command_pool, primary);
            break;
        case GPUQueueType::COMPUTE:
            command_buffer.command_queue  = rhi->compute_queue;
            command_buffer.command_buffer = allocate_command_buffer(compute_command_pool, primary);
            break;
        case GPUQueueType::TRANSFER:
            command_buffer.command_queue  = rhi->transfer_queue;
            command_buffer.command_buffer = allocate_command_buffer(transfer_command_pool, primary);
            break;
    }

    uint index  = static_cast<uint>(allocated_command_buffers.size());
    auto handle = GPUCommandEncoderHandle(index);
    allocated_command_buffers.push_back(command_buffer);
    return handle;
}

void VulkanFrame::destroy()
{
    delete_vkfence(inflight_fence);
    delete_fence(image_available_semaphore);
    delete_fence(render_complete_semaphore);

    if (compute_command_pool != VK_NULL_HANDLE) {
        reset_command_pool(compute_command_pool);
        delete_command_pool(compute_command_pool);
    }

    if (graphics_command_pool != VK_NULL_HANDLE) {
        reset_command_pool(graphics_command_pool);
        delete_command_pool(graphics_command_pool);
    }

    if (transfer_command_pool != VK_NULL_HANDLE) {
        reset_command_pool(transfer_command_pool);
        delete_command_pool(transfer_command_pool);
    }
}
