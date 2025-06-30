// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

void D3D12CommandBuffer::wait(const D3D12Fence& fence, GPUBarrierSyncFlags)
{
    // NOTE: D3D12 does not support fine-grain control at pipeline stage level,
    // therefore sync flags is not used.

    wait_fences.push_back(FenceOps{
        fence.fence,
        fence.target,
    });
}

void D3D12CommandBuffer::signal(const D3D12Fence& fence, GPUBarrierSyncFlags)
{
    // NOTE: D3D12 does not support fine-grain control at pipeline stage level,
    // therefore sync flags is not used.

    signal_fences.push_back(FenceOps{
        fence.fence,
        fence.target++,
    });
}

void D3D12CommandBuffer::submit()
{
    // GPU wait - make the command queue wait for the fence (current value)
    for (auto& fence : wait_fences)
        command_queue->Wait(fence.fence, fence.value);

    // submit the command list to the queue
    ID3D12CommandList* command_lists[] = {command_buffer};
    command_queue->ExecuteCommandLists(1, command_lists);

    // signal from GPU - command queue will signal when it reaches this point
    for (auto& fence : signal_fences)
        command_queue->Signal(fence.fence, fence.value);
}

void D3D12CommandBuffer::begin()
{
    command_buffer->Reset(command_allocator, nullptr);
}

void D3D12CommandBuffer::end()
{
    ThrowIfFailed(command_buffer->Close());
}

void cmd::wait_fence(GPUCommandEncoderHandle cmdbuffer, GPUFenceHandle fence, GPUBarrierSyncFlags sync)
{
    auto  rhi = get_rhi();
    auto& fen = fetch_resource(rhi->fences, fence);
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    cmd.wait(fen, sync);
}

void cmd::signal_fence(GPUCommandEncoderHandle cmdbuffer, GPUFenceHandle fence, GPUBarrierSyncFlags sync)
{
    auto  rhi = get_rhi();
    auto& fen = fetch_resource(rhi->fences, fence);
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    cmd.signal(fen, sync);
}

void cmd::begin_render_pass(GPUCommandEncoderHandle cmdbuffer, const GPURenderPassDescriptor& descriptor)
{
    assert(!descriptor.color_attachments.empty());

    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    UINT max_render_targets = std::min((uint)D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, (uint)descriptor.color_attachments.size());
    UINT num_render_targets = 0;
    bool has_depth_stencil  = descriptor.depth_stencil_attachment.view.valid();

    // set up render pass parameters
    D3D12_RENDER_PASS_RENDER_TARGET_DESC rt_descs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    for (uint i = 0; i < max_render_targets; i++) {
        auto& attachment = descriptor.color_attachments[i];
        auto  view       = fetch_resource(rhi->views, attachment.view).rtv_view.handle;
        auto& rt_desc    = rt_descs[num_render_targets];

        // set CPU descriptor handle for the RTV
        rt_desc.cpuDescriptor = view;

        // set beginning access
        if (attachment.load_op == GPULoadOp::CLEAR) {
            rt_desc.BeginningAccess.Type                      = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            rt_desc.BeginningAccess.Clear.ClearValue.Color[0] = attachment.clear_value.r;
            rt_desc.BeginningAccess.Clear.ClearValue.Color[1] = attachment.clear_value.g;
            rt_desc.BeginningAccess.Clear.ClearValue.Color[2] = attachment.clear_value.b;
            rt_desc.BeginningAccess.Clear.ClearValue.Color[3] = attachment.clear_value.a;
        } else if (attachment.load_op == GPULoadOp::LOAD) {
            rt_desc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
        } else { // GPULoadOp::DONT_CARE
            rt_desc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }

        // set ending access
        if (attachment.store_op == GPUStoreOp::STORE) {
            rt_desc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        } else {
            rt_desc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }

        num_render_targets++;
    }

    // set up depth stencil parameters
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC ds_desc = {};
    if (has_depth_stencil) {
        auto view = fetch_resource(rhi->views, descriptor.depth_stencil_attachment.view).dsv_view.handle;

        // set CPU descriptor handle for the DSV
        ds_desc.cpuDescriptor = view;

        // set depth beginning access
        if (descriptor.depth_stencil_attachment.depth_load_op == GPULoadOp::CLEAR) {
            ds_desc.DepthBeginningAccess.Type                                = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            ds_desc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = descriptor.depth_stencil_attachment.depth_clear_value;
        } else if (descriptor.depth_stencil_attachment.depth_load_op == GPULoadOp::LOAD) {
            ds_desc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
        } else {
            ds_desc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }

        // set stencil beginning access
        if (descriptor.depth_stencil_attachment.stencil_load_op == GPULoadOp::CLEAR) {
            ds_desc.StencilBeginningAccess.Type                                  = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            ds_desc.StencilBeginningAccess.Clear.ClearValue.DepthStencil.Stencil = descriptor.depth_stencil_attachment.stencil_clear_value;
        } else if (descriptor.depth_stencil_attachment.stencil_load_op == GPULoadOp::LOAD) {
            ds_desc.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
        } else {
            ds_desc.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }

        // set depth ending access
        if (descriptor.depth_stencil_attachment.depth_store_op == GPUStoreOp::STORE) {
            ds_desc.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        } else {
            ds_desc.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }

        // set stencil ending access
        if (descriptor.depth_stencil_attachment.stencil_store_op == GPUStoreOp::STORE) {
            ds_desc.StencilEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
        } else {
            ds_desc.StencilEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }
    }

    // NOTE: Use default viewport and scissor

    // begin the render pass
    // D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES - if you need UAV writes during render pass
    // D3D12_RENDER_PASS_FLAG_SUSPENDING_PASS - for tiled rendering scenarios
    // D3D12_RENDER_PASS_FLAG_RESUMING_PASS - for resuming suspended passes
    D3D12_RENDER_PASS_FLAGS flags = D3D12_RENDER_PASS_FLAG_NONE;

    ID3D12GraphicsCommandList4* command_list = static_cast<ID3D12GraphicsCommandList4*>(cmd.command_buffer);
    command_list->BeginRenderPass(
        num_render_targets,
        num_render_targets > 0 ? rt_descs : nullptr,
        has_depth_stencil ? &ds_desc : nullptr,
        flags);
}

void cmd::end_render_pass(GPUCommandEncoderHandle cmdbuffer)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);

    ID3D12GraphicsCommandList4* command_list = static_cast<ID3D12GraphicsCommandList4*>(cmd.command_buffer);
    command_list->EndRenderPass();
}

void cmd::set_render_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURenderPipelineHandle pipeline)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& pip = fetch_resource(rhi->pipelines, pipeline);

    if (cmd.last_bound.pipeline != pip.pipeline) {
        cmd.last_bound.pipeline    = pip.pipeline;
        cmd.last_bound.layouts     = pip.layout.bind_group_layouts.data();
        cmd.last_bound.num_layouts = static_cast<uint>(pip.layout.bind_group_layouts.size());
        cmd.command_buffer->SetGraphicsRootSignature(pip.layout.layout);
        cmd.command_buffer->SetPipelineState(pip.pipeline);
    }
}

void cmd::set_compute_pipeline(GPUCommandEncoderHandle cmdbuffer, GPUComputePipelineHandle pipeline)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& pip = fetch_resource(rhi->pipelines, pipeline);

    if (cmd.last_bound.pipeline != pip.pipeline) {
        cmd.last_bound.pipeline    = pip.pipeline;
        cmd.last_bound.layouts     = pip.layout.bind_group_layouts.data();
        cmd.last_bound.num_layouts = static_cast<uint>(pip.layout.bind_group_layouts.size());
        cmd.command_buffer->SetComputeRootSignature(pip.layout.layout);
        cmd.command_buffer->SetPipelineState(pip.pipeline);
    }
}

void cmd::set_raytracing_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURayTracingPipelineHandle pipeline)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& pip = fetch_resource(rhi->pipelines, pipeline);

    if (cmd.last_bound.pipeline != pip.pipeline) {
        cmd.last_bound.pipeline    = pip.pipeline;
        cmd.last_bound.layouts     = pip.layout.bind_group_layouts.data();
        cmd.last_bound.num_layouts = static_cast<uint>(pip.layout.bind_group_layouts.size());
        cmd.command_buffer->SetComputeRootSignature(pip.layout.layout);
        cmd.command_buffer->SetPipelineState(pip.pipeline);
    }
}

void cmd::set_bind_group(GPUCommandEncoderHandle cmdbuffer, GPUIndex32 index, GPUBindGroupHandle bind_group, const Vector<GPUBufferDynamicOffset>& dynamic_offsets)
{
    auto  rhi = get_rhi();
    auto& frm = rhi->current_frame();
    auto& cmd = frm.command(cmdbuffer);
    auto  des = frm.descriptor(bind_group);

    assert(index < cmd.last_bound.num_layouts);
    auto& lay = cmd.last_bound.layouts[index];
    for (auto& binding : lay.bindings) {
        if (binding.heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
            auto handle = des.sampler_base.value().gpu_handle;
            handle.ptr += binding.base_offset * frm.sampler_heap.increment;
            cmd.command_buffer->SetGraphicsRootDescriptorTable(
                binding.root_param_index,
                handle);
        } else {
            auto handle = des.cbv_srv_uav_base.value().gpu_handle;
            handle.ptr += binding.base_offset * frm.cbv_srv_uav_heap.increment;
            cmd.command_buffer->SetGraphicsRootDescriptorTable(
                binding.root_param_index,
                handle);
        }
    }
}

void cmd::set_index_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle buffer, GPUIndexFormat format, GPUSize64 offset, GPUSize64 size)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& buf = fetch_resource(rhi->buffers, buffer);

    // create the index buffer view
    D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
    index_buffer_view.BufferLocation          = buf.buffer->GetGPUVirtualAddress() + offset;
    index_buffer_view.SizeInBytes             = static_cast<UINT>(size);
    index_buffer_view.Format                  = d3d12enum(format);

    // Set the index buffer
    cmd.command_buffer->IASetIndexBuffer(&index_buffer_view);
}

void cmd::set_vertex_buffer(GPUCommandEncoderHandle cmdbuffer, GPUIndex32 slot, GPUBufferHandle buffer, GPUSize64 offset, GPUSize64 size)
{
    auto  rhi = get_rhi();
    auto& cmd = rhi->current_frame().command(cmdbuffer);
    auto& buf = fetch_resource(rhi->buffers, buffer);

    // create vertex buffer view
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
    vertex_buffer_view.BufferLocation           = buf.buffer->GetGPUVirtualAddress() + offset;
    vertex_buffer_view.SizeInBytes              = static_cast<UINT>(size);
    vertex_buffer_view.StrideInBytes            = 0; // stride needs to be set based on vertex format

    // set the vertex buffer at the specified slot
    cmd.command_buffer->IASetVertexBuffers(slot, 1, &vertex_buffer_view);
}

// void cmd::draw(GPUCommandEncoderHandle cmdbuffer, GPUSize32 vertex_count, GPUSize32 instance_count, GPUSize32 first_vertex, GPUSize32 first_instance)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     rhi->vtable.vkCmdDraw(cmd.command_buffer, vertex_count, instance_count, first_vertex, first_instance);
// }
//
// void cmd::draw_indexed(GPUCommandEncoderHandle cmdbuffer, GPUSize32 index_count, GPUSize32 instance_count, GPUSize32 first_index, GPUSignedOffset32 base_vertex, GPUSize32 first_instance)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     rhi->vtable.vkCmdDrawIndexed(cmd.command_buffer, index_count, instance_count, first_index, base_vertex, first_instance);
// }
//
// void cmd::draw_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset, GPUSize32 draw_count)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& buf = fetch_resource(rhi->buffers, indirect_buffer);
//     rhi->vtable.vkCmdDrawIndirect(cmd.command_buffer, buf.buffer, indirect_offset, draw_count, static_cast<uint32_t>(sizeof(VkDrawIndirectCommand)));
// }
//
// void cmd::draw_indexed_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset, GPUSize32 draw_count)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& buf = fetch_resource(rhi->buffers, indirect_buffer);
//     rhi->vtable.vkCmdDrawIndexedIndirect(cmd.command_buffer, buf.buffer, indirect_offset, draw_count, static_cast<uint32_t>(sizeof(VkDrawIndexedIndirectCommand)));
// }
//
// void cmd::dispatch_workgroups(GPUCommandEncoderHandle cmdbuffer, GPUSize32 x, GPUSize32 y, GPUSize32 z)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     rhi->vtable.vkCmdDispatch(cmd.command_buffer, x, y, z);
// }
//
// void cmd::dispatch_workgroups_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& buf = fetch_resource(rhi->buffers, indirect_buffer);
//     rhi->vtable.vkCmdDispatchIndirect(cmd.command_buffer, buf.buffer, indirect_offset);
// }
//
// void cmd::copy_buffer_to_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle source, GPUSize64 source_offset, GPUBufferHandle destination, GPUSize64 destination_offset, GPUSize64 size)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& src = fetch_resource(rhi->buffers, source);
//     auto& dst = fetch_resource(rhi->buffers, destination);
//
//     auto copy      = VkBufferCopy{};
//     copy.size      = size;
//     copy.srcOffset = source_offset;
//     copy.dstOffset = destination_offset;
//
//     rhi->vtable.vkCmdCopyBuffer(cmd.command_buffer, src.buffer, dst.buffer, 1, &copy);
// }
//
// void cmd::copy_buffer_to_texture(GPUCommandEncoderHandle cmdbuffer, const GPUTexelCopyBufferInfo& source, const GPUTexelCopyTextureInfo& destination, GPUExtent3D copy_size)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& src = fetch_resource(rhi->buffers, source.buffer);
//     auto& dst = fetch_resource(rhi->textures, destination.texture);
//
//     auto copy                            = VkBufferImageCopy{};
//     copy.bufferOffset                    = source.offset;
//     copy.bufferRowLength                 = source.bytes_per_row;
//     copy.bufferImageHeight               = source.rows_per_image; // TODO: convert this to texels, not in bytes
//     copy.imageOffset.x                   = destination.origin.x;
//     copy.imageOffset.y                   = destination.origin.y;
//     copy.imageOffset.z                   = destination.origin.z;
//     copy.imageExtent.width               = copy_size.width;
//     copy.imageExtent.height              = copy_size.height;
//     copy.imageExtent.depth               = copy_size.depth;
//     copy.imageSubresource.aspectMask     = vkenum(destination.aspect);
//     copy.imageSubresource.mipLevel       = destination.mip_level;
//     copy.imageSubresource.baseArrayLayer = 0; // TODO: Is WebGPU able to set this?
//     copy.imageSubresource.layerCount     = 1; // TODO: Is WebGPU able to set this?
//
//     auto layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//     rhi->vtable.vkCmdCopyBufferToImage(cmd.command_buffer, src.buffer, dst.image, layout, 1, &copy);
// }
//
// void cmd::copy_texture_to_buffer(GPUCommandEncoderHandle cmdbuffer, const GPUTexelCopyTextureInfo& source, const GPUTexelCopyBufferInfo& destination, const GPUExtent3D& copy_size)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& src = fetch_resource(rhi->textures, source.texture);
//     auto& dst = fetch_resource(rhi->buffers, destination.buffer);
//
//     auto copy                            = VkBufferImageCopy{};
//     copy.bufferOffset                    = destination.offset;
//     copy.bufferRowLength                 = destination.bytes_per_row; // TODO: convert this to texels, not in bytes
//     copy.bufferImageHeight               = destination.rows_per_image;
//     copy.imageOffset.x                   = source.origin.x;
//     copy.imageOffset.y                   = source.origin.y;
//     copy.imageOffset.z                   = source.origin.z;
//     copy.imageExtent.width               = copy_size.width;
//     copy.imageExtent.height              = copy_size.height;
//     copy.imageExtent.depth               = copy_size.depth;
//     copy.imageSubresource.aspectMask     = vkenum(source.aspect);
//     copy.imageSubresource.mipLevel       = source.mip_level;
//     copy.imageSubresource.baseArrayLayer = 0; // TODO: Is WebGPU able to set this?
//     copy.imageSubresource.layerCount     = 1; // TODO: Is WebGPU able to set this?
//
//     auto layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//     rhi->vtable.vkCmdCopyImageToBuffer(cmd.command_buffer, src.image, layout, dst.buffer, 1, &copy);
// }
//
// void cmd::copy_texture_to_texture(GPUCommandEncoderHandle cmdbuffer, const GPUTexelCopyTextureInfo& source, const GPUTexelCopyTextureInfo& destination, const GPUExtent3D& copy_size)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& src = fetch_resource(rhi->textures, source.texture);
//     auto& dst = fetch_resource(rhi->textures, destination.texture);
//
//     auto copy                          = VkImageCopy{};
//     copy.extent.width                  = copy_size.width;
//     copy.extent.height                 = copy_size.height;
//     copy.extent.depth                  = copy_size.depth;
//     copy.srcOffset.x                   = source.origin.x;
//     copy.srcOffset.y                   = source.origin.y;
//     copy.srcOffset.z                   = source.origin.z;
//     copy.dstOffset.x                   = destination.origin.x;
//     copy.dstOffset.y                   = destination.origin.y;
//     copy.dstOffset.z                   = destination.origin.z;
//     copy.srcSubresource.aspectMask     = vkenum(source.aspect);
//     copy.srcSubresource.mipLevel       = source.mip_level;
//     copy.srcSubresource.baseArrayLayer = 0; // TODO: Is WebGPU able to set this?
//     copy.srcSubresource.layerCount     = 1; // TODO: Is WebGPU able to set this?
//     copy.dstSubresource.aspectMask     = vkenum(destination.aspect);
//     copy.dstSubresource.mipLevel       = source.mip_level;
//     copy.dstSubresource.baseArrayLayer = 0; // TODO: Is WebGPU able to set this?
//     copy.dstSubresource.layerCount     = 1; // TODO: Is WebGPU able to set this?
//
//     auto src_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//     auto dst_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//     rhi->vtable.vkCmdCopyImage(cmd.command_buffer, src.image, src_layout, dst.image, dst_layout, 1, &copy);
// }
//
// void cmd::clear_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle buffer, GPUSize64 offset, GPUSize64 size)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& buf = fetch_resource(rhi->buffers, buffer);
//
//     rhi->vtable.vkCmdFillBuffer(
//         cmd.command_buffer,
//         buf.buffer,
//         offset,
//         size == 0 ? VK_WHOLE_SIZE : size,
//         0);
// }
//
// void cmd::set_viewport(GPUCommandEncoderHandle cmdbuffer, float x, float y, float w, float h, float min_depth, float max_depth)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     auto viewport     = VkViewport{};
//     viewport.x        = x;
//     viewport.y        = y + h;
//     viewport.width    = w;
//     viewport.height   = -h;
//     viewport.minDepth = min_depth;
//     viewport.maxDepth = max_depth;
//
//     rhi->vtable.vkCmdSetViewport(cmd.command_buffer, 0, 1, &viewport);
// }
//
// void cmd::set_scissor_rect(GPUCommandEncoderHandle cmdbuffer, GPUIntegerCoordinate x, GPUIntegerCoordinate y, GPUIntegerCoordinate w, GPUIntegerCoordinate h)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     auto scissor          = VkRect2D{};
//     scissor.offset.x      = x;
//     scissor.offset.y      = y;
//     scissor.extent.width  = w;
//     scissor.extent.height = h;
//
//     rhi->vtable.vkCmdSetScissor(cmd.command_buffer, 0, 1, &scissor);
// }
//
// void cmd::set_blend_constant(GPUCommandEncoderHandle cmdbuffer, GPUColor color)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     float blend_color[4] = {
//         color.r,
//         color.g,
//         color.b,
//         color.a,
//     };
//
//     rhi->vtable.vkCmdSetBlendConstants(cmd.command_buffer, blend_color);
// }
//
// void cmd::set_stencil_reference(GPUCommandEncoderHandle cmdbuffer, GPUStencilValue reference)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     // NOTE: WebGPU applies stencil reference to both front and back uniformly to simplify the design.
//     auto face = VK_STENCIL_FRONT_AND_BACK;
//     rhi->vtable.vkCmdSetStencilReference(cmd.command_buffer, face, reference);
// }
//
// void cmd::begin_occlusion_query(GPUCommandEncoderHandle cmdbuffer, GPUSize32 query_index)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     assert(cmd.query_set.valid());
//
//     cmd.query_index = query_index;
//     rhi->vtable.vkCmdBeginQuery(cmd.command_buffer, cmd.query_set.pool, query_index, 0);
// }
//
// void cmd::end_occlusion_query(GPUCommandEncoderHandle cmdbuffer)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     assert(cmd.query_set.valid());
//     assert(cmd.query_index.has_value());
//
//     rhi->vtable.vkCmdEndQuery(cmd.command_buffer, cmd.query_set.pool, cmd.query_index.value());
// }
//
// void cmd::write_timestamp(GPUCommandEncoderHandle cmdbuffer, GPUQuerySetHandle query_set, GPUSize32 query_index)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& qry = fetch_resource(rhi->query_sets, query_set);
//
//     rhi->vtable.vkCmdWriteTimestamp(cmd.command_buffer, VK_PIPELINE_STAGE_NONE, qry.pool, query_index);
// }
//
// void cmd::write_blas_properties(GPUCommandEncoderHandle cmdbuffer, GPUQuerySetHandle query_set, GPUSize32 query_index, GPUBlasHandle blas)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& qry = fetch_resource(rhi->query_sets, query_set);
//     auto& bvh = fetch_resource(rhi->blases, blas);
//
//     rhi->vtable.vkCmdWriteAccelerationStructuresPropertiesKHR(cmd.command_buffer, 1, &bvh.blas, qry.type, qry.pool, query_index);
// }
//
// void cmd::resolve_query_set(GPUCommandEncoderHandle cmdbuffer, GPUQuerySetHandle query_set, GPUSize32 first_query, GPUSize32 query_count, GPUBufferHandle destination, GPUSize64 destination_offset)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& qry = fetch_resource(rhi->query_sets, query_set);
//     auto& buf = fetch_resource(rhi->buffers, destination);
//
//     auto stride = sizeof(uint64_t); // NOTE: This might not be good enough. We might determine the stride based on query type.
//     rhi->vtable.vkCmdCopyQueryPoolResults(cmd.command_buffer, qry.pool, first_query, query_count, buf.buffer, destination_offset, stride, 0);
// }
//
// void cmd::memory_barrier(GPUCommandEncoderHandle cmdbuffer, uint32_t count, GPUMemoryBarrier* barriers)
// {
//     Vector<VkMemoryBarrier2KHR> bars;
//     for (uint32_t i = 0; i < count; i++) {
//         auto& barrier = barriers[i];
//
//         auto b          = VkMemoryBarrier2KHR{};
//         b.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
//         b.pNext         = nullptr;
//         b.srcStageMask  = vkenum(barrier.src_sync);
//         b.dstStageMask  = vkenum(barrier.dst_sync);
//         b.srcAccessMask = vkenum(barrier.src_access);
//         b.dstAccessMask = vkenum(barrier.dst_access);
//         bars.push_back(b);
//     }
//
//     auto dependency                     = VkDependencyInfoKHR{};
//     dependency.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
//     dependency.pNext                    = nullptr;
//     dependency.dependencyFlags          = 0;
//     dependency.memoryBarrierCount       = static_cast<uint32_t>(bars.size());
//     dependency.pMemoryBarriers          = bars.data();
//     dependency.bufferMemoryBarrierCount = 0;
//     dependency.pBufferMemoryBarriers    = nullptr;
//     dependency.imageMemoryBarrierCount  = 0;
//     dependency.pImageMemoryBarriers     = nullptr;
//
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     rhi->vtable.vkCmdPipelineBarrier2KHR(cmd.command_buffer, &dependency);
// }
//
// void cmd::buffer_barrier(GPUCommandEncoderHandle cmdbuffer, uint32_t count, GPUBufferBarrier* barriers)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     Vector<VkBufferMemoryBarrier2KHR> bars;
//     for (uint32_t i = 0; i < count; i++) {
//         auto& barrier = barriers[i];
//
//         auto b                = VkBufferMemoryBarrier2KHR{};
//         b.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR;
//         b.pNext               = nullptr;
//         b.srcStageMask        = vkenum(barrier.src_sync);
//         b.dstStageMask        = vkenum(barrier.dst_sync);
//         b.srcAccessMask       = vkenum(barrier.src_access);
//         b.dstAccessMask       = vkenum(barrier.dst_access);
//         b.buffer              = fetch_resource(rhi->buffers, barrier.buffer).buffer;
//         b.offset              = barrier.offset;
//         b.size                = barrier.size;
//         b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL;
//         bars.push_back(b);
//     }
//
//     auto dependency                     = VkDependencyInfoKHR{};
//     dependency.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
//     dependency.pNext                    = nullptr;
//     dependency.dependencyFlags          = 0;
//     dependency.memoryBarrierCount       = 0;
//     dependency.pMemoryBarriers          = nullptr;
//     dependency.bufferMemoryBarrierCount = static_cast<uint32_t>(bars.size());
//     dependency.pBufferMemoryBarriers    = bars.data();
//     dependency.imageMemoryBarrierCount  = 0;
//     dependency.pImageMemoryBarriers     = nullptr;
//
//     rhi->vtable.vkCmdPipelineBarrier2KHR(cmd.command_buffer, &dependency);
// }
//
// void cmd::texture_barrier(GPUCommandEncoderHandle cmdbuffer, uint32_t count, GPUTextureBarrier* barriers)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//
//     Vector<VkImageMemoryBarrier2KHR> bars;
//     for (uint32_t i = 0; i < count; i++) {
//         auto& barrier = barriers[i];
//
//         auto& t         = fetch_resource(rhi->textures, barrier.texture);
//         auto  b         = VkImageMemoryBarrier2KHR{};
//         b.sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR;
//         b.pNext         = nullptr;
//         b.srcStageMask  = vkenum(barrier.src_sync);
//         b.dstStageMask  = vkenum(barrier.dst_sync);
//         b.srcAccessMask = vkenum(barrier.src_access);
//         b.dstAccessMask = vkenum(barrier.dst_access);
//         b.image         = t.image;
//         b.oldLayout     = vkenum(barrier.src_layout);
//         b.newLayout     = vkenum(barrier.dst_layout);
//
//         b.subresourceRange.aspectMask     = vkenum(t.aspects);
//         b.subresourceRange.baseArrayLayer = barrier.subresources.base_array_layer;
//         b.subresourceRange.baseMipLevel   = barrier.subresources.base_mip_level;
//         b.subresourceRange.layerCount     = barrier.subresources.array_layers;
//         b.subresourceRange.levelCount     = barrier.subresources.mip_level_count;
//         bars.push_back(b);
//     }
//
//     auto dependency                     = VkDependencyInfoKHR{};
//     dependency.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
//     dependency.pNext                    = nullptr;
//     dependency.dependencyFlags          = 0;
//     dependency.memoryBarrierCount       = 0;
//     dependency.pMemoryBarriers          = nullptr;
//     dependency.bufferMemoryBarrierCount = 0;
//     dependency.pBufferMemoryBarriers    = nullptr;
//     dependency.imageMemoryBarrierCount  = static_cast<uint32_t>(bars.size());
//     dependency.pImageMemoryBarriers     = bars.data();
//
//     rhi->vtable.vkCmdPipelineBarrier2KHR(cmd.command_buffer, &dependency);
// }
//
// void cmd::build_tlases(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle scratch_buffer, uint32_t count, GPUTlasBuildEntry* entries)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& buf = fetch_resource(rhi->buffers, scratch_buffer);
//
//     Vector<VkBufferMemoryBarrier>                       barriers;
//     Vector<VkAccelerationStructureBuildGeometryInfoKHR> build_infos  = {};
//     Vector<VkAccelerationStructureBuildRangeInfoKHR*>   build_ranges = {};
//
//     barriers.reserve(count);
//     build_infos.reserve(count);
//     build_ranges.reserve(count);
//
//     VkDeviceAddress scratch_address = buf.device_address;
//     for (uint i = 0; i < count; i++) {
//         auto& entry = entries[i];
//         auto& tlas  = fetch_resource(rhi->tlases, entry.tlas);
//
//         // create VkAccelerationStructureInstanceKHR on the fly
//         auto address = tlas.staging.map<VkAccelerationStructureInstanceKHR>();
//         for (auto& instance : entry.instances) {
//             auto& blas = fetch_resource(rhi->blases, instance.blas);
//
//             float* src_transform = reinterpret_cast<float*>(instance.transform);
//             float* dst_transform = reinterpret_cast<float*>(address->transform.matrix);
//             std::memcpy(dst_transform, src_transform, 12);
//             address->instanceCustomIndex                    = instance.custom_data;
//             address->instanceShaderBindingTableRecordOffset = 0; // NOTE: We don't support more complex cases for now.
//             address->mask                                   = instance.mask;
//             address->flags                                  = blas.build.flags;
//             address->accelerationStructureReference         = blas.reference;
//
//             address++;
//         }
//         tlas.staging.unmap();
//
//         // copy from staging buffer to instance buffer
//         auto copy      = VkBufferCopy{};
//         copy.size      = entry.instances.size() * sizeof(VkAccelerationStructureInstanceKHR);
//         copy.srcOffset = 0;
//         copy.dstOffset = 0;
//         rhi->vtable.vkCmdCopyBuffer(cmd.command_buffer, tlas.staging.buffer, tlas.instances.buffer, 1, &copy);
//
//         // prepare buffer barrier
//         barriers.push_back({});
//         auto& barrier               = barriers.back();
//         barrier.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
//         barrier.pNext               = nullptr;
//         barrier.offset              = 0u;
//         barrier.size                = copy.size;
//         barrier.buffer              = tlas.instances.buffer;
//         barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
//         barrier.dstAccessMask       = VK_ACCESS_TRANSFER_READ_BIT;
//         barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL;
//         barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_EXTERNAL;
//
//         // prepare build info
//         tlas.build.srcAccelerationStructure  = tlas.tlas;
//         tlas.build.dstAccelerationStructure  = tlas.tlas;
//         tlas.build.geometryCount             = 1;
//         tlas.build.pGeometries               = &tlas.geometry;
//         tlas.build.scratchData.deviceAddress = scratch_address;
//         tlas.build.mode                      = (tlas.tlas == VK_NULL_HANDLE)
//                                                    ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR
//                                                    : vkenum(tlas.update_mode);
//
//         // prepare build range
//         tlas.range.firstVertex     = 0;
//         tlas.range.primitiveCount  = entry.instances.size();
//         tlas.range.primitiveOffset = 0;
//         tlas.range.transformOffset = 0;
//
//         build_infos.push_back(tlas.build);
//         build_ranges.push_back(&tlas.range);
//
//         scratch_address += (tlas.build.mode == VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR)
//                                ? tlas.sizes.buildScratchSize
//                                : tlas.sizes.updateScratchSize;
//     }
//
//     // wait until copies have completed
//     rhi->vtable.vkCmdPipelineBarrier(
//         cmd.command_buffer,
//         VK_PIPELINE_STAGE_TRANSFER_BIT,
//         VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
//         VK_DEPENDENCY_BY_REGION_BIT,
//         0, nullptr,
//         static_cast<uint32_t>(barriers.size()), barriers.data(),
//         0, nullptr);
//
//     rhi->vtable.vkCmdBuildAccelerationStructuresKHR(
//         cmd.command_buffer,
//         static_cast<uint32_t>(build_infos.size()),
//         build_infos.data(),
//         build_ranges.data());
// }
//
// void cmd::build_blases(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle scratch_buffer, uint32_t count, GPUBlasBuildEntry* entries)
// {
//     auto  rhi = get_rhi();
//     auto& cmd = rhi->current_frame().command(cmdbuffer);
//     auto& buf = fetch_resource(rhi->buffers, scratch_buffer);
//
//     Vector<VkAccelerationStructureBuildGeometryInfoKHR> build_infos  = {};
//     Vector<VkAccelerationStructureBuildRangeInfoKHR*>   build_ranges = {};
//
//     build_infos.reserve(count);
//     build_ranges.reserve(count);
//
//     VkDeviceAddress scratch_address = buf.device_address;
//     for (uint i = 0; i < count; i++) {
//         auto& entry = entries[i];
//         auto& blas  = fetch_resource(rhi->blases, entry.blas);
//
//         // geometry count check
//         if (entries->geometries.triangles.size() > blas.geometries.size()) {
//             get_logger()->error("Trying to build more geometries than BVH could hold!");
//         }
//
//         // update geometries and ranges
//         uint geometry_count = std::min(entries->geometries.triangles.size(), blas.geometries.size());
//         for (uint k = 0; k < geometry_count; k++) {
//             auto& src_geometry = entry.geometries.triangles.at(k);
//             auto& dst_geometry = blas.geometries.at(k);
//
//             auto index_data = VkDeviceAddress{0};
//             if (src_geometry.index_buffer.valid())
//                 index_data = fetch_resource(rhi->buffers, src_geometry.index_buffer).device_address;
//
//             auto vertex_data = VkDeviceAddress{0};
//             if (src_geometry.vertex_buffer.valid())
//                 vertex_data = fetch_resource(rhi->buffers, src_geometry.vertex_buffer).device_address;
//
//             dst_geometry.sType                                       = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
//             dst_geometry.geometryType                                = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
//             dst_geometry.geometry.triangles.sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
//             dst_geometry.geometry.triangles.indexType                = vkenum(src_geometry.size.index_format);
//             dst_geometry.geometry.triangles.indexData.deviceAddress  = index_data;
//             dst_geometry.geometry.triangles.vertexStride             = src_geometry.vertex_stride;
//             dst_geometry.geometry.triangles.vertexFormat             = vkenum(src_geometry.size.vertex_format);
//             dst_geometry.geometry.triangles.vertexData.deviceAddress = vertex_data;
//             dst_geometry.geometry.triangles.maxVertex                = src_geometry.size.vertex_count - 1;
//
//             auto& range           = blas.ranges.at(k);
//             range.firstVertex     = src_geometry.first_vertex;
//             range.primitiveCount  = src_geometry.size.vertex_count / 3;
//             range.primitiveOffset = src_geometry.first_index;
//             range.transformOffset = 0;
//         }
//
//         // update build info
//         blas.build.srcAccelerationStructure  = blas.blas;
//         blas.build.dstAccelerationStructure  = blas.blas;
//         blas.build.scratchData.deviceAddress = scratch_address;
//         blas.build.geometryCount             = geometry_count;
//         blas.build.pGeometries               = blas.geometries.data();
//         blas.build.mode                      = (blas.blas == VK_NULL_HANDLE)
//                                                    ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR
//                                                    : vkenum(blas.update_mode);
//
//         build_infos.push_back(blas.build);
//         build_ranges.push_back(blas.ranges.data());
//
//         scratch_address += (blas.build.mode == VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR)
//                                ? blas.sizes.buildScratchSize
//                                : blas.sizes.updateScratchSize;
//     }
//
//     rhi->vtable.vkCmdBuildAccelerationStructuresKHR(
//         cmd.command_buffer,
//         static_cast<uint32_t>(build_infos.size()),
//         build_infos.data(),
//         build_ranges.data());
// }
