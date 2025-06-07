#include "VkUtils.h"

void VulkanCommandBuffer::wait(const VulkanSemaphore& fence, GPUBarrierSyncFlags sync)
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

void VulkanCommandBuffer::signal(const VulkanSemaphore& fence, GPUBarrierSyncFlags sync)
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

    vk_check(rhi->vtable.vkQueueSubmit2KHR(command_queue, 1, &submit_info, fence.fence));
}

void VulkanCommandBuffer::begin()
{
    auto rhi = get_rhi();

    auto begin_info             = VkCommandBufferBeginInfo{};
    begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext            = nullptr;
    begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    begin_info.pInheritanceInfo = nullptr;

    vk_check(rhi->vtable.vkBeginCommandBuffer(command_buffer, &begin_info));
}

void VulkanCommandBuffer::end()
{
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkEndCommandBuffer(command_buffer));
}

void cmd::wait_fence(GPUCommandEncoderHandle cmdbuffer, GPUFenceHandle fence, GPUBarrierSyncFlags sync)
{
    auto  rhi = get_rhi();
    auto& sem = fetch_resource(rhi->fences, fence);
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    cmd.wait(sem, sync);
}

void cmd::signal_fence(GPUCommandEncoderHandle cmdbuffer, GPUFenceHandle fence, GPUBarrierSyncFlags sync)
{
    auto  rhi = get_rhi();
    auto& sem = fetch_resource(rhi->fences, fence);
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    cmd.signal(sem, sync);
}

void cmd::begin_render_pass(GPUCommandEncoderHandle cmdbuffer, const GPURenderPassDescriptor& descriptor)
{
    assert(!descriptor.color_attachments.empty());

    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    Vector<VkRenderingAttachmentInfo> color_attachments;
    for (auto& attachment : descriptor.color_attachments) {
        color_attachments.push_back({});

        auto& desc                       = color_attachments.back();
        desc.sType                       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        desc.pNext                       = nullptr;
        desc.imageLayout                 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        desc.loadOp                      = vkenum(attachment.load_op);
        desc.storeOp                     = vkenum(attachment.store_op);
        desc.imageView                   = fetch_resource(rhi->views, attachment.view).view;
        desc.resolveImageView            = VK_NULL_HANDLE;
        desc.resolveMode                 = VK_RESOLVE_MODE_NONE;
        desc.clearValue.color.float32[0] = attachment.clear_value.r;
        desc.clearValue.color.float32[1] = attachment.clear_value.g;
        desc.clearValue.color.float32[2] = attachment.clear_value.b;
        desc.clearValue.color.float32[3] = attachment.clear_value.a;
    }

    VkRenderingAttachmentInfo depth_attachment{};
    depth_attachment.sType                         = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.pNext                         = nullptr;
    depth_attachment.imageLayout                   = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depth_attachment.loadOp                        = vkenum(descriptor.depth_stencil_attachment.depth_load_op);
    depth_attachment.storeOp                       = vkenum(descriptor.depth_stencil_attachment.depth_store_op);
    depth_attachment.resolveImageView              = VK_NULL_HANDLE;
    depth_attachment.resolveMode                   = VK_RESOLVE_MODE_NONE;
    depth_attachment.clearValue.depthStencil.depth = descriptor.depth_stencil_attachment.depth_clear_value;
    depth_attachment.imageView                     = descriptor.depth_stencil_attachment.view.valid()
                                                         ? fetch_resource(rhi->views, descriptor.depth_stencil_attachment.view).view
                                                         : VK_NULL_HANDLE;

    VkRenderingAttachmentInfo stencil_attachment{};
    stencil_attachment.sType                           = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    stencil_attachment.pNext                           = nullptr;
    stencil_attachment.imageLayout                     = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    stencil_attachment.loadOp                          = vkenum(descriptor.depth_stencil_attachment.stencil_load_op);
    stencil_attachment.storeOp                         = vkenum(descriptor.depth_stencil_attachment.stencil_store_op);
    stencil_attachment.resolveImageView                = VK_NULL_HANDLE;
    stencil_attachment.resolveMode                     = VK_RESOLVE_MODE_NONE;
    stencil_attachment.clearValue.depthStencil.stencil = descriptor.depth_stencil_attachment.stencil_clear_value;
    stencil_attachment.imageView                       = descriptor.depth_stencil_attachment.view.valid()
                                                             ? fetch_resource(rhi->views, descriptor.depth_stencil_attachment.view).view
                                                             : VK_NULL_HANDLE;

    auto& render_view         = fetch_resource(rhi->views, descriptor.color_attachments.at(0).view);
    auto  render_area         = VkRect2D{};
    render_area.offset.x      = 0;
    render_area.offset.y      = 0;
    render_area.extent.width  = render_view.extent.width;
    render_area.extent.height = render_view.extent.height;

    auto rendering                 = VkRenderingInfo{};
    rendering.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering.pNext                = nullptr;
    rendering.renderArea           = render_area;
    rendering.layerCount           = 1;
    rendering.colorAttachmentCount = static_cast<uint32_t>(descriptor.color_attachments.size());
    rendering.pColorAttachments    = color_attachments.data();
    rendering.pDepthAttachment     = nullptr; // &depth_attachment;
    rendering.pStencilAttachment   = nullptr; // &stencil_attachment;

    rhi->vtable.vkCmdBeginRenderingKHR(cmd.command_buffer, &rendering);
}

void cmd::end_render_pass(GPUCommandEncoderHandle cmdbuffer)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    rhi->vtable.vkCmdEndRenderingKHR(cmd.command_buffer);
}

void cmd::set_render_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURenderPipelineHandle pipeline, GPUPipelineLayoutHandle layout)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& pip = fetch_resource(rhi->pipelines, pipeline);
    auto& lay = fetch_resource(rhi->pipeline_layouts, layout);

    cmd.last_bound_point    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    cmd.last_bound_layout   = lay.layout;
    cmd.last_bound_pipeline = pip.pipeline;

    rhi->vtable.vkCmdBindPipeline(cmd.command_buffer, cmd.last_bound_point, cmd.last_bound_pipeline);
}

void cmd::set_compute_pipeline(GPUCommandEncoderHandle cmdbuffer, GPUComputePipelineHandle pipeline, GPUPipelineLayoutHandle layout)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& pip = fetch_resource(rhi->pipelines, pipeline);
    auto& lay = fetch_resource(rhi->pipeline_layouts, layout);

    cmd.last_bound_point    = VK_PIPELINE_BIND_POINT_COMPUTE;
    cmd.last_bound_layout   = lay.layout;
    cmd.last_bound_pipeline = pip.pipeline;

    rhi->vtable.vkCmdBindPipeline(cmd.command_buffer, cmd.last_bound_point, cmd.last_bound_pipeline);
}

void cmd::set_raytracing_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURayTracingPipelineHandle pipeline, GPUPipelineLayoutHandle layout)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& pip = fetch_resource(rhi->pipelines, pipeline);
    auto& lay = fetch_resource(rhi->pipeline_layouts, layout);

    cmd.last_bound_point    = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
    cmd.last_bound_layout   = lay.layout;
    cmd.last_bound_pipeline = pip.pipeline;

    rhi->vtable.vkCmdBindPipeline(cmd.command_buffer, cmd.last_bound_point, cmd.last_bound_pipeline);
}

void cmd::set_bind_group(GPUCommandEncoderHandle cmdbuffer, GPUIndex32 index, GPUBindGroupHandle bind_group, const Vector<GPUBufferDynamicOffset>& dynamic_offsets)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto  des = rhi->current_frame().descriptor(bind_group);
    rhi->vtable.vkCmdBindDescriptorSets(cmd.command_buffer, cmd.last_bound_point, cmd.last_bound_layout,
        index, 1, &des,
        static_cast<uint32_t>(dynamic_offsets.size()), dynamic_offsets.data());
}

void cmd::set_index_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle buffer, GPUIndexFormat format, GPUSize64 offset, GPUSize64 size)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& buf = fetch_resource(rhi->buffers, buffer);
    if (size == 0) {
        rhi->vtable.vkCmdBindIndexBuffer(cmd.command_buffer, buf.buffer, offset, vkenum(format));
    } else {
        rhi->vtable.vkCmdBindIndexBuffer2(cmd.command_buffer, buf.buffer, offset, size, vkenum(format));
    }
}

void cmd::set_vertex_buffer(GPUCommandEncoderHandle cmdbuffer, GPUIndex32 slot, GPUBufferHandle buffer, GPUSize64 offset, GPUSize64 size)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& buf = fetch_resource(rhi->buffers, buffer);
    if (size == 0) {
        rhi->vtable.vkCmdBindVertexBuffers(cmd.command_buffer, slot, 1, &buf.buffer, &offset);
    } else {
        rhi->vtable.vkCmdBindVertexBuffers2(cmd.command_buffer, slot, 1, &buf.buffer, &offset, &size, nullptr);
    }
}

void cmd::draw(GPUCommandEncoderHandle cmdbuffer, GPUSize32 vertex_count, GPUSize32 instance_count, GPUSize32 first_vertex, GPUSize32 first_instance)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    rhi->vtable.vkCmdDraw(cmd.command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

void cmd::draw_indexed(GPUCommandEncoderHandle cmdbuffer, GPUSize32 index_count, GPUSize32 instance_count, GPUSize32 first_index, GPUSignedOffset32 base_vertex, GPUSize32 first_instance)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    rhi->vtable.vkCmdDrawIndexed(cmd.command_buffer, index_count, instance_count, first_index, base_vertex, first_instance);
}

void cmd::draw_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset, GPUSize32 draw_count)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& buf = fetch_resource(rhi->buffers, indirect_buffer);
    rhi->vtable.vkCmdDrawIndirect(cmd.command_buffer, buf.buffer, indirect_offset, draw_count, static_cast<uint32_t>(sizeof(VkDrawIndirectCommand)));
}

void cmd::draw_indexed_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset, GPUSize32 draw_count)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& buf = fetch_resource(rhi->buffers, indirect_buffer);
    rhi->vtable.vkCmdDrawIndexedIndirect(cmd.command_buffer, buf.buffer, indirect_offset, draw_count, static_cast<uint32_t>(sizeof(VkDrawIndexedIndirectCommand)));
}

void cmd::dispatch_workgroups(GPUCommandEncoderHandle cmdbuffer, GPUSize32 x, GPUSize32 y, GPUSize32 z)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    rhi->vtable.vkCmdDispatch(cmd.command_buffer, x, y, z);
}

void cmd::dispatch_workgroups_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& buf = fetch_resource(rhi->buffers, indirect_buffer);
    rhi->vtable.vkCmdDispatchIndirect(cmd.command_buffer, buf.buffer, indirect_offset);
}

void cmd::copy_buffer_to_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle source, GPUSize64 source_offset, GPUBufferHandle destination, GPUSize64 destination_offset, GPUSize64 size)
{
    assert(!!!"unimplemented");
}

void cmd::copy_buffer_to_texture(GPUCommandEncoderHandle cmdbuffer, const GPUTexelCopyBufferInfo& source, const GPUTexelCopyTextureInfo& destination, GPUExtent3D copy_size)
{
    assert(!!!"unimplemented");
}

void cmd::copy_texture_to_buffer(GPUCommandEncoderHandle cmdbuffer, const GPUTexelCopyTextureInfo& source, const GPUTexelCopyBufferInfo& destination, const GPUExtent3D& copy_size)
{
    assert(!!!"unimplemented");
}

void cmd::copy_texture_to_texture(GPUCommandEncoderHandle cmdbuffer, const GPUTexelCopyTextureInfo& source, const GPUTexelCopyTextureInfo& destination, const GPUExtent3D& copy_size)
{
    assert(!!!"unimplemented");
}

void cmd::clear_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle buffer, GPUSize64 offset, GPUSize64 size)
{
    assert(!!!"unimplemented");
}

void cmd::resolve_query_set(GPUCommandEncoderHandle cmdbuffer, GPUQuerySetHandle query_set, GPUSize32 first_query, GPUSize32 query_count, GPUBufferHandle destination, GPUSize64 destination_offset)
{
    assert(!!!"unimplemented");
}

void cmd::set_viewport(GPUCommandEncoderHandle cmdbuffer, float x, float y, float w, float h, float min_depth, float max_depth)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    auto viewport     = VkViewport{};
    viewport.x        = x;
    viewport.y        = y + h;
    viewport.width    = w;
    viewport.height   = -h;
    viewport.minDepth = min_depth;
    viewport.maxDepth = max_depth;

    rhi->vtable.vkCmdSetViewport(cmd.command_buffer, 0, 1, &viewport);
}

void cmd::set_scissor_rect(GPUCommandEncoderHandle cmdbuffer, GPUIntegerCoordinate x, GPUIntegerCoordinate y, GPUIntegerCoordinate w, GPUIntegerCoordinate h)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    auto scissor          = VkRect2D{};
    scissor.offset.x      = x;
    scissor.offset.y      = y;
    scissor.extent.width  = w;
    scissor.extent.height = h;

    rhi->vtable.vkCmdSetScissor(cmd.command_buffer, 0, 1, &scissor);
}

void cmd::set_blend_constant(GPUCommandEncoderHandle cmdbuffer, GPUColor color)
{
    assert(!!!"unimplemented");
}

void cmd::set_stencil_reference(GPUCommandEncoderHandle cmdbuffer, GPUStencilValue reference)
{
    assert(!!!"unimplemented");
}

void cmd::begin_occlusion_query(GPUCommandEncoderHandle cmdbuffer, GPUSize32 queryIndex)
{
    assert(!!!"unimplemented");
}

void cmd::end_occlusion_query(GPUCommandEncoderHandle cmdbuffer)
{
    assert(!!!"unimplemented");
}

void cmd::memory_barrier(GPUCommandEncoderHandle cmdbuffer, uint32_t count, GPUMemoryBarrier* barriers)
{
    Vector<VkMemoryBarrier2KHR> bars;
    for (uint32_t i = 0; i < count; i++) {
        auto& barrier = barriers[i];

        auto b          = VkMemoryBarrier2KHR{};
        b.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
        b.pNext         = nullptr;
        b.srcStageMask  = vkenum(barrier.src_sync);
        b.dstStageMask  = vkenum(barrier.src_sync);
        b.srcAccessMask = vkenum(barrier.src_access);
        b.dstAccessMask = vkenum(barrier.src_access);
        bars.push_back(b);
    }

    auto dependency                     = VkDependencyInfoKHR{};
    dependency.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
    dependency.pNext                    = nullptr;
    dependency.dependencyFlags          = 0;
    dependency.memoryBarrierCount       = static_cast<uint32_t>(bars.size());
    dependency.pMemoryBarriers          = bars.data();
    dependency.bufferMemoryBarrierCount = 0;
    dependency.pBufferMemoryBarriers    = nullptr;
    dependency.imageMemoryBarrierCount  = 0;
    dependency.pImageMemoryBarriers     = nullptr;

    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    rhi->vtable.vkCmdPipelineBarrier2KHR(cmd.command_buffer, &dependency);
}

void cmd::buffer_barrier(GPUCommandEncoderHandle cmdbuffer, uint32_t count, GPUBufferBarrier* barriers)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    Vector<VkBufferMemoryBarrier2KHR> bars;
    for (uint32_t i = 0; i < count; i++) {
        auto& barrier = barriers[i];

        auto b                = VkBufferMemoryBarrier2KHR{};
        b.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
        b.pNext               = nullptr;
        b.srcStageMask        = vkenum(barrier.src_sync);
        b.dstStageMask        = vkenum(barrier.src_sync);
        b.srcAccessMask       = vkenum(barrier.src_access);
        b.dstAccessMask       = vkenum(barrier.src_access);
        b.buffer              = fetch_resource(rhi->buffers, barrier.buffer).buffer;
        b.offset              = barrier.offset;
        b.size                = barrier.size;
        b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL;
        bars.push_back(b);
    }

    auto dependency                     = VkDependencyInfoKHR{};
    dependency.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
    dependency.pNext                    = nullptr;
    dependency.dependencyFlags          = 0;
    dependency.memoryBarrierCount       = 0;
    dependency.pMemoryBarriers          = nullptr;
    dependency.bufferMemoryBarrierCount = static_cast<uint32_t>(bars.size());
    dependency.pBufferMemoryBarriers    = bars.data();
    dependency.imageMemoryBarrierCount  = 0;
    dependency.pImageMemoryBarriers     = nullptr;

    rhi->vtable.vkCmdPipelineBarrier2KHR(cmd.command_buffer, &dependency);
}

void cmd::texture_barrier(GPUCommandEncoderHandle cmdbuffer, uint32_t count, GPUTextureBarrier* barriers)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    Vector<VkImageMemoryBarrier2KHR> bars;
    for (uint32_t i = 0; i < count; i++) {
        auto& barrier = barriers[i];

        auto& t         = fetch_resource(rhi->textures, barrier.texture);
        auto  b         = VkImageMemoryBarrier2KHR{};
        b.sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
        b.pNext         = nullptr;
        b.srcStageMask  = vkenum(barrier.src_sync);
        b.dstStageMask  = vkenum(barrier.src_sync);
        b.srcAccessMask = vkenum(barrier.src_access);
        b.dstAccessMask = vkenum(barrier.src_access);
        b.image         = t.image;
        b.oldLayout     = vkenum(barrier.src_layout);
        b.newLayout     = vkenum(barrier.dst_layout);

        b.subresourceRange.aspectMask     = vkenum(t.aspects);
        b.subresourceRange.baseArrayLayer = barrier.subresources.base_array_layer;
        b.subresourceRange.baseMipLevel   = barrier.subresources.base_mip_level;
        b.subresourceRange.layerCount     = barrier.subresources.layer_count;
        b.subresourceRange.levelCount     = barrier.subresources.level_count;
        bars.push_back(b);
    }

    auto dependency                     = VkDependencyInfoKHR{};
    dependency.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
    dependency.pNext                    = nullptr;
    dependency.dependencyFlags          = 0;
    dependency.memoryBarrierCount       = 0;
    dependency.pMemoryBarriers          = nullptr;
    dependency.bufferMemoryBarrierCount = 0;
    dependency.pBufferMemoryBarriers    = nullptr;
    dependency.imageMemoryBarrierCount  = static_cast<uint32_t>(bars.size());
    dependency.pImageMemoryBarriers     = bars.data();

    rhi->vtable.vkCmdPipelineBarrier2KHR(cmd.command_buffer, &dependency);
}
