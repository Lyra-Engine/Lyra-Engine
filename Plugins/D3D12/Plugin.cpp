#include "D3D12Utils.h"

// global module headers
#include <Lyra/Common/String.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/Render/RHI/API.h>
#include <Lyra/Window/API.h>

using namespace lyra;
using namespace lyra::rhi;
using namespace lyra::wsi;

auto get_api_name() -> CString { return "D3D12"; }

// bool api::get_surface_extent(GPUExtent2D& extent)
// {
//     auto rhi = get_rhi();
//
//     extent.width  = rhi->swapchain_extent.width;
//     extent.height = rhi->swapchain_extent.height;
//     return true;
// }
//
// bool api::get_surface_format(GPUTextureFormat& format)
// {
//     auto rhi = get_rhi();
//     switch (rhi->swapchain_format) {
//         case VK_FORMAT_B8G8R8A8_SRGB:
//             format = GPUTextureFormat::BGRA8UNORM_SRGB;
//             return true;
//         default:
//             throw std::runtime_error("Failed to match the corresponding swapchain format");
//             return false;
//     }
// }
//
// bool api::create_buffer(GPUBufferHandle& buffer, const GPUBufferDescriptor& desc)
// {
//     auto obj = VulkanBuffer(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->buffers.add(obj);
//
//     buffer = GPUBufferHandle(ind);
//     return true;
// }
//
// void api::delete_buffer(GPUBufferHandle buffer)
// {
//     get_rhi()->buffers.remove(buffer.value);
// }
//
// void api::map_buffer(GPUBufferHandle buffer, GPUMapMode, GPUSize64 offset, GPUSize64 size)
// {
//     auto  rhi = get_rhi();
//     auto& buf = fetch_resource(rhi->buffers, buffer);
//     buf.map(offset, size);
// }
//
// void api::unmap_buffer(GPUBufferHandle buffer)
// {
//     auto  rhi = get_rhi();
//     auto& buf = fetch_resource(rhi->buffers, buffer);
//     buf.unmap();
//     buf.mapped_size = 0;
// }
//
// void api::get_mapped_range(GPUBufferHandle buffer, MappedBufferRange& range)
// {
//     auto  rhi  = get_rhi();
//     auto& buf  = fetch_resource(rhi->buffers, buffer);
//     range.data = buf.mapped_data;
//     range.size = buf.mapped_size;
// }
//
// bool api::create_sampler(GPUSamplerHandle& sampler, const GPUSamplerDescriptor& desc)
// {
//     auto obj = VulkanSampler(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->samplers.add(obj);
//
//     sampler = GPUSamplerHandle(ind);
//     return true;
// }
//
// void api::delete_sampler(GPUSamplerHandle sampler)
// {
//     get_rhi()->samplers.remove(sampler.value);
// }
//
// bool api::create_texture(GPUTextureHandle& texture, const GPUTextureDescriptor& desc)
// {
//     auto obj = VulkanTexture(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->textures.add(obj);
//
//     texture = GPUTextureHandle(ind);
//     return true;
// }
//
// void api::delete_texture(GPUTextureHandle handle)
// {
//     get_rhi()->textures.remove(handle.value);
// }
//
// bool api::create_texture_view(GPUTextureViewHandle& handle, GPUTextureHandle texture, const GPUTextureViewDescriptor& desc)
// {
//     auto  rhi = get_rhi();
//     auto& tex = fetch_resource(rhi->textures, texture);
//     auto  obj = VulkanTextureView(tex, desc);
//     auto  ind = rhi->views.add(obj);
//
//     handle = GPUTextureViewHandle(ind);
//     return true;
// }
//
// bool api::create_shader_module(GPUShaderModuleHandle& shader, const GPUShaderModuleDescriptor& desc)
// {
//     auto obj = VulkanShader(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->shaders.add(obj);
//
//     shader = GPUShaderModuleHandle(ind);
//     return true;
// }
//
// void api::delete_shader_module(GPUShaderModuleHandle shader)
// {
//     get_rhi()->shaders.remove(shader.value);
// }
//
// bool api::create_fence(GPUFenceHandle& fence, VkSemaphoreType type)
// {
//     auto obj = VulkanSemaphore(type);
//     auto rhi = get_rhi();
//     auto ind = rhi->fences.add(obj);
//
//     fence = GPUFenceHandle(ind);
//     return true;
// }
//
// bool api::create_fence(GPUFenceHandle& fence)
// {
//     auto obj = VulkanSemaphore(VK_SEMAPHORE_TYPE_TIMELINE);
//     auto rhi = get_rhi();
//     auto ind = rhi->fences.add(obj);
//
//     fence = GPUFenceHandle(ind);
//     return true;
// }
//
// void api::delete_fence(GPUFenceHandle fence)
// {
//     get_rhi()->fences.remove(fence.value);
// }
//
// bool api::create_blas(GPUBlasHandle& blas, const GPUBlasDescriptor& desc, const Vector<GPUBlasGeometrySizeDescriptor>& sizes)
// {
//     auto obj = VulkanBlas(desc, sizes);
//     auto rhi = get_rhi();
//     auto ind = rhi->blases.add(obj);
//
//     blas = GPUBlasHandle(ind);
//     return true;
// }
//
// void api::delete_blas(GPUBlasHandle blas)
// {
//     get_rhi()->blases.remove(blas.value);
// }
//
// bool api::create_tlas(GPUTlasHandle& tlas, const GPUTlasDescriptor& desc)
// {
//     auto obj = VulkanTlas(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->tlases.add(obj);
//
//     tlas = GPUTlasHandle(ind);
//     return true;
// }
//
// void api::delete_tlas(GPUTlasHandle tlas)
// {
//     get_rhi()->tlases.remove(tlas.value);
// }
//
// bool api::get_blas_sizes(GPUBlasHandle blas, GPUBVHSizes& sizes)
// {
//     auto& size_info   = fetch_resource(get_rhi()->blases, blas).sizes;
//     sizes.bvh_size    = size_info.accelerationStructureSize;
//     sizes.build_size  = size_info.buildScratchSize;
//     sizes.update_size = size_info.updateScratchSize;
//     return true;
// }
//
// bool api::get_tlas_sizes(GPUTlasHandle tlas, GPUBVHSizes& sizes)
// {
//     auto& size_info   = fetch_resource(get_rhi()->tlases, tlas).sizes;
//     sizes.bvh_size    = size_info.accelerationStructureSize;
//     sizes.build_size  = size_info.buildScratchSize;
//     sizes.update_size = size_info.updateScratchSize;
//     return true;
// }
//
// bool api::create_query_set(GPUQuerySetHandle& query_set, const GPUQuerySetDescriptor& desc)
// {
//     auto obj = VulkanQuerySet(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->query_sets.add(obj);
//
//     query_set = GPUQuerySetHandle(ind);
//     return true;
// }
//
// void api::delete_query_set(GPUQuerySetHandle query_set)
// {
//     get_rhi()->query_sets.remove(query_set.value);
// }
//
// bool api::create_bind_group_layout(GPUBindGroupLayoutHandle& layout, const GPUBindGroupLayoutDescriptor& desc)
// {
//     auto obj = VulkanBindGroupLayout(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->bind_group_layouts.add(obj);
//
//     layout = GPUBindGroupLayoutHandle(ind);
//     return true;
// }
//
// void api::delete_bind_group_layout(GPUBindGroupLayoutHandle layout)
// {
//     get_rhi()->bind_group_layouts.remove(layout.value);
// }
//
// bool api::create_pipeline_layout(GPUPipelineLayoutHandle& layout, const GPUPipelineLayoutDescriptor& desc)
// {
//     auto obj = VulkanPipelineLayout(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->pipeline_layouts.add(obj);
//
//     layout = GPUPipelineLayoutHandle(ind);
//     return true;
// }
//
// void api::delete_pipeline_layout(GPUPipelineLayoutHandle layout)
// {
//     get_rhi()->pipeline_layouts.remove(layout.value);
// }
//
// bool api::create_render_pipeline(GPURenderPipelineHandle& pipeline, const GPURenderPipelineDescriptor& desc)
// {
//     auto obj = VulkanPipeline(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->pipelines.add(obj);
//
//     pipeline = GPURenderPipelineHandle(ind);
//     return true;
// }
//
// void api::delete_render_pipeline(GPURenderPipelineHandle pipeline)
// {
//     get_rhi()->pipelines.remove(pipeline.value);
// }
//
// bool api::create_compute_pipeline(GPUComputePipelineHandle& pipeline, const GPUComputePipelineDescriptor& desc)
// {
//     auto obj = VulkanPipeline(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->pipelines.add(obj);
//
//     pipeline = GPUComputePipelineHandle(ind);
//     return true;
// }
//
// void api::delete_compute_pipeline(GPUComputePipelineHandle pipeline)
// {
//     get_rhi()->pipelines.remove(pipeline.value);
// }
//
// bool api::create_raytracing_pipeline(GPURayTracingPipelineHandle& handle, const GPURayTracingPipelineDescriptor& desc)
// {
//     auto obj = VulkanPipeline(desc);
//     auto rhi = get_rhi();
//     auto ind = rhi->pipelines.add(obj);
//
//     handle = GPURayTracingPipelineHandle(ind);
//     return true;
// }
//
// void api::delete_raytracing_pipeline(GPURayTracingPipelineHandle pipeline)
// {
//     get_rhi()->pipelines.remove(pipeline.value);
// }
//
// bool api::create_bind_group(GPUBindGroupHandle& bind_group, const GPUBindGroupDescriptor& desc)
// {
//     bind_group = ::create_bind_group(desc);
//     return true;
// }
//
// bool api::create_command_buffer(GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBufferDescriptor& descriptor)
// {
//     auto  rhi = get_rhi();
//     auto& frm = rhi->current_frame();
//     cmdbuffer = frm.allocate(descriptor.queue, true);
//     frm.command(cmdbuffer).begin();
//     return true;
// }
//
// bool api::create_command_bundle(GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBundleDescriptor& descriptor)
// {
//     auto  rhi = get_rhi();
//     auto& frm = rhi->current_frame();
//     cmdbuffer = frm.allocate(descriptor.queue, false);
//     frm.command(cmdbuffer).begin();
//     return true;
// }
//
// bool api::submit_command_buffer(GPUCommandEncoderHandle cmdbuffer)
// {
//     auto  rhi = get_rhi();
//     auto& frm = rhi->current_frame();
//     auto& cmd = frm.command(cmdbuffer);
//     cmd.end();
//     cmd.submit();
//     return true;
// }
//
// void api::wait_idle()
// {
//     auto rhi = get_rhi();
//     vk_check(rhi->vtable.vkDeviceWaitIdle(rhi->device));
//
//     // optional: clean up all pools from all frames
//     for (auto& frame : rhi->frames)
//         frame.reset(true);
// }
//
// void api::wait_fence(GPUFenceHandle handle)
// {
//     auto rhi = get_rhi();
//     fetch_resource(rhi->fences, handle).wait();
// }

LYRA_EXPORT auto prepare() -> void
{
    // vk_check(volkInitialize());
}

LYRA_EXPORT auto cleanup() -> void
{
}

LYRA_EXPORT auto create() -> RenderAPI
{
    auto api = RenderAPI{};
    // api.create_instance                  = api::create_instance;
    // api.delete_instance                  = api::delete_instance;
    // api.create_adapter                   = api::create_adapter;
    // api.delete_adapter                   = api::delete_adapter;
    // api.create_device                    = api::create_device;
    // api.delete_device                    = api::delete_device;
    // api.create_surface                   = api::create_surface;
    // api.delete_surface                   = api::delete_surface;
    // api.get_surface_extent               = api::get_surface_extent;
    // api.get_surface_format               = api::get_surface_format;
    // api.create_buffer                    = api::create_buffer;
    // api.delete_buffer                    = api::delete_buffer;
    // api.create_texture                   = api::create_texture;
    // api.delete_texture                   = api::delete_texture;
    // api.create_texture_view              = api::create_texture_view;
    // api.create_sampler                   = api::create_sampler;
    // api.delete_sampler                   = api::delete_sampler;
    // api.create_fence                     = api::create_fence;
    // api.delete_fence                     = api::delete_fence;
    // api.create_shader_module             = api::create_shader_module;
    // api.delete_shader_module             = api::delete_shader_module;
    // api.create_blas                      = api::create_blas;
    // api.delete_blas                      = api::delete_blas;
    // api.create_tlas                      = api::create_tlas;
    // api.delete_tlas                      = api::delete_tlas;
    // api.create_pipeline_layout           = api::create_pipeline_layout;
    // api.delete_pipeline_layout           = api::delete_pipeline_layout;
    // api.create_render_pipeline           = api::create_render_pipeline;
    // api.delete_render_pipeline           = api::delete_render_pipeline;
    // api.create_compute_pipeline          = api::create_compute_pipeline;
    // api.delete_compute_pipeline          = api::delete_compute_pipeline;
    // api.create_raytracing_pipeline       = api::create_raytracing_pipeline;
    // api.delete_raytracing_pipeline       = api::delete_raytracing_pipeline;
    // api.create_bind_group                = api::create_bind_group;
    // api.create_bind_group_layout         = api::create_bind_group_layout;
    // api.delete_bind_group_layout         = api::delete_bind_group_layout;
    // api.wait_idle                        = api::wait_idle;
    // api.wait_fence                       = api::wait_fence;
    // api.map_buffer                       = api::map_buffer;
    // api.unmap_buffer                     = api::unmap_buffer;
    // api.get_mapped_range                 = api::get_mapped_range;
    // api.create_command_buffer            = api::create_command_buffer;
    // api.create_command_bundle            = api::create_command_bundle;
    // api.submit_command_buffer            = api::submit_command_buffer;
    // api.get_blas_sizes                   = api::get_blas_sizes;
    // api.get_tlas_sizes                   = api::get_tlas_sizes;
    // api.acquire_next_frame               = api::acquire_next_frame;
    // api.present_curr_frame               = api::present_curr_frame;
    // api.cmd_wait_fence                   = cmd::wait_fence;
    // api.cmd_signal_fence                 = cmd::signal_fence;
    // api.cmd_begin_render_pass            = cmd::begin_render_pass;
    // api.cmd_end_render_pass              = cmd::end_render_pass;
    // api.cmd_set_render_pipeline          = cmd::set_render_pipeline;
    // api.cmd_set_compute_pipeline         = cmd::set_compute_pipeline;
    // api.cmd_set_raytracing_pipeline      = cmd::set_raytracing_pipeline;
    // api.cmd_set_bind_group               = cmd::set_bind_group;
    // api.cmd_set_index_buffer             = cmd::set_index_buffer;
    // api.cmd_set_vertex_buffer            = cmd::set_vertex_buffer;
    // api.cmd_draw                         = cmd::draw;
    // api.cmd_draw_indexed                 = cmd::draw_indexed;
    // api.cmd_draw_indirect                = cmd::draw_indirect;
    // api.cmd_draw_indexed_indirect        = cmd::draw_indexed_indirect;
    // api.cmd_dispatch_workgroups          = cmd::dispatch_workgroups;
    // api.cmd_dispatch_workgroups_indirect = cmd::dispatch_workgroups_indirect;
    // api.cmd_copy_buffer_to_buffer        = cmd::copy_buffer_to_buffer;
    // api.cmd_copy_buffer_to_texture       = cmd::copy_buffer_to_texture;
    // api.cmd_copy_texture_to_buffer       = cmd::copy_texture_to_buffer;
    // api.cmd_copy_texture_to_texture      = cmd::copy_texture_to_texture;
    // api.cmd_clear_buffer                 = cmd::clear_buffer;
    // api.cmd_set_viewport                 = cmd::set_viewport;
    // api.cmd_set_scissor_rect             = cmd::set_scissor_rect;
    // api.cmd_set_blend_constant           = cmd::set_blend_constant;
    // api.cmd_set_stencil_reference        = cmd::set_stencil_reference;
    // api.cmd_begin_occlusion_query        = cmd::begin_occlusion_query;
    // api.cmd_end_occlusion_query          = cmd::end_occlusion_query;
    // api.cmd_write_timestamp              = cmd::write_timestamp;
    // api.cmd_write_blas_properties        = cmd::write_blas_properties;
    // api.cmd_resolve_query_set            = cmd::resolve_query_set;
    // api.cmd_memory_barrier               = cmd::memory_barrier;
    // api.cmd_buffer_barrier               = cmd::buffer_barrier;
    // api.cmd_texture_barrier              = cmd::texture_barrier;
    return api;
}
