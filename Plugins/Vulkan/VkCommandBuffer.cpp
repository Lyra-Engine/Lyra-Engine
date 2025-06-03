#include "VkUtils.h"

void VulkanCommandBuffer::wait(const VulkanFence& fence, GPUBarrierSyncFlags sync)
{
    auto rhi = get_rhi();
    wait_semaphores.push_back(VkSemaphoreSubmitInfo{});

    auto& submit_info       = wait_semaphores.back();
    submit_info.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submit_info.pNext       = nullptr;
    submit_info.semaphore   = fence.semaphore;
    submit_info.value       = fence.type == VK_SEMAPHORE_TYPE_BINARY ? 0 : fence.target;
    submit_info.stageMask   = vkenum(sync);
    submit_info.deviceIndex = 0;
}

void VulkanCommandBuffer::signal(const VulkanFence& fence, GPUBarrierSyncFlags sync)
{
    auto rhi = get_rhi();
    signal_semaphores.push_back(VkSemaphoreSubmitInfo{});

    auto& submit_info       = signal_semaphores.back();
    submit_info.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submit_info.pNext       = nullptr;
    submit_info.semaphore   = fence.semaphore;
    submit_info.value       = fence.type == VK_SEMAPHORE_TYPE_BINARY ? 0 : fence.target;
    submit_info.stageMask   = vkenum(sync);
    submit_info.deviceIndex = 0;

    // attach the inflight fence if this command buffer signals render complete semaphore
    auto& frame     = rhi->current_frame();
    auto& semaphore = fetch_resource(rhi->fences, frame.render_complete_semaphore);
    if (fence.semaphore == semaphore.semaphore)
        this->fence = frame.inflight_fence;
}

void VulkanCommandBuffer::submit()
{
    auto rhi = get_rhi();

    auto cmd_submit_info          = VkCommandBufferSubmitInfo{};
    cmd_submit_info.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_submit_info.pNext         = nullptr;
    cmd_submit_info.commandBuffer = command_buffer;
    cmd_submit_info.deviceMask    = 0;

    auto submit_info                     = VkSubmitInfo2{};
    submit_info.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.pNext                    = nullptr;
    submit_info.flags                    = 0;
    submit_info.commandBufferInfoCount   = 1;
    submit_info.pCommandBufferInfos      = &cmd_submit_info;
    submit_info.waitSemaphoreInfoCount   = static_cast<uint32_t>(wait_semaphores.size());
    submit_info.pWaitSemaphoreInfos      = wait_semaphores.data();
    submit_info.signalSemaphoreInfoCount = static_cast<uint32_t>(signal_semaphores.size());
    submit_info.pSignalSemaphoreInfos    = signal_semaphores.data();

    vk_check(rhi->vtable.vkQueueSubmit2KHR(command_queue, 1, &submit_info, fence));
}

void VulkanCommandBuffer::begin()
{
    begin_command_buffer(command_buffer);
}

void VulkanCommandBuffer::end()
{
    end_command_buffer(command_buffer);
}

bool create_command_buffer(GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBufferDescriptor& descriptor)
{
    auto  rhi   = get_rhi();
    auto& frame = rhi->current_frame();
    cmdbuffer   = frame.allocate(descriptor.queue, true);
    return true;
}

bool create_command_bundle(GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBufferDescriptor& descriptor)
{
    auto  rhi   = get_rhi();
    auto& frame = rhi->current_frame();
    cmdbuffer   = frame.allocate(descriptor.queue, false);
    return true;
}

void begin_command_buffer(VkCommandBuffer cmdbuffer)
{
    auto rhi = get_rhi();

    auto begin_info             = VkCommandBufferBeginInfo{};
    begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext            = nullptr;
    begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    begin_info.pInheritanceInfo = nullptr;

    vk_check(rhi->vtable.vkBeginCommandBuffer(cmdbuffer, &begin_info));
}

void end_command_buffer(VkCommandBuffer cmdbuffer)
{
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkEndCommandBuffer(cmdbuffer));
}

void cmd::set_render_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURenderPipelineHandle pipeline)
{
}

void cmd::set_compute_pipeline(GPUCommandEncoderHandle cmdbuffer, GPUComputePipelineHandle pipeline)
{
}

void cmd::set_raytracing_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURayTracingPipelineHandle pipeline)
{
}

void cmd::set_bind_group(GPUCommandEncoderHandle cmdbuffer, GPUPipelineLayoutHandle layout, GPUIndex32 index, GPUBindGroupHandle bind_group, const Vector<GPUBufferDynamicOffset>& dynamic_offsets)
{
}

void cmd::set_index_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle buffer, GPUIndexFormat format, GPUSize64 offset, GPUSize64 size)
{
}

void cmd::set_vertex_buffer(GPUCommandEncoderHandle cmdbuffer, GPUIndex32 slot, GPUBufferHandle buffer, GPUSize64 offset, GPUSize64 size)
{
}

void cmd::draw(GPUCommandEncoderHandle cmdbuffer, GPUSize32 vertex_count, GPUSize32 instance_count, GPUSize32 first_vertex, GPUSize32 first_instance)
{
}

void cmd::draw_indexed(GPUCommandEncoderHandle cmdbuffer, GPUSize32 index_count, GPUSize32 instance_count, GPUSize32 first_index, GPUSignedOffset32 base_vertex, GPUSize32 first_instance)
{
}

void cmd::draw_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset)
{
}

void cmd::draw_indexed_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset)
{
}
