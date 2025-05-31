#ifndef LYRA_LIBRARY_RENDER_RHI_API_H
#define LYRA_LIBRARY_RENDER_RHI_API_H

#include <Render/RHI/Enums.h>
#include <Render/RHI/Utils.h>
#include <Render/RHI/Descs.h>
#include <Render/RHI/Error.h>
#include <Render/RHI/Types.h>

namespace lyra::rhi
{
    struct RenderAPI
    {
        // api name
        CString (*get_api_name)();

        bool (*create_instance)(const RHIDescriptor& descriptor);
        void (*delete_instance)();

        bool (*create_adapter)(GPUAdapter& adapter, const GPUAdapterDescriptor& descriptor);
        void (*delete_adapter)();

        bool (*create_surface)(GPUSurface& surface, const GPUSurfaceDescriptor& descriptor);
        void (*delete_surface)();

        bool (*create_device)(const GPUDeviceDescriptor& descriptor);
        void (*delete_device)();

        bool (*create_fence)(GPUFenceHandle& fence);
        void (*delete_fence)(GPUFenceHandle fence);

        bool (*create_buffer)(GPUBufferHandle& buffer, const GPUBufferDescriptor& descriptor);
        void (*delete_buffer)(GPUBufferHandle buffer);

        bool (*create_sampler)(GPUSamplerHandle& sampler, const GPUSamplerDescriptor& descriptor);
        void (*delete_sampler)(GPUSamplerHandle sampler);

        bool (*create_texture)(GPUTextureHandle& texture, const GPUTextureDescriptor& descriptor);
        void (*delete_texture)(GPUTextureHandle texture);

        bool (*create_texture_view)(GPUTextureHandle texture, GPUTextureViewHandle& view, const GPUTextureViewDescriptor& descriptor);
        void (*delete_texture_view)(GPUTextureHandle texture, GPUTextureViewHandle view);

        bool (*create_shader_module)(GPUShaderModuleHandle& texture, const GPUShaderModuleDescriptor& descriptor);
        void (*delete_shader_module)(GPUShaderModuleHandle texture);

        bool (*create_blas)(GPUBlasHandle& texture, const GPUBlasDescriptor& descriptor);
        void (*delete_blas)(GPUBlasHandle texture);

        bool (*create_tlas)(GPUTlasHandle& texture, const GPUTlasDescriptor& descriptor);
        void (*delete_tlas)(GPUTlasHandle texture);

        bool (*create_bind_group_layout)(GPUBindGroupLayoutHandle& layout, const GPUBindGroupLayoutDescriptor& descriptor);
        void (*delete_bind_group_layout)(GPUBindGroupLayoutHandle layout);

        bool (*create_pipeline_layout)(GPUPipelineLayoutHandle& layout, const GPUPipelineLayoutDescriptor& descriptor);
        void (*delete_pipeline_layout)(GPUBindGroupLayoutHandle layout);

        bool (*create_render_pipeline)(GPURenderPipelineHandle& texture, const GPURenderPipelineDescriptor& descriptor);
        void (*delete_render_pipeline)(GPURenderPipelineHandle texture);

        bool (*create_compute_pipeline)(GPUComputePipelineHandle& texture, const GPUComputePipelineDescriptor& descriptor);
        void (*delete_compute_pipeline)(GPUComputePipelineHandle texture);

        bool (*create_raytracing_pipeline)(GPURayTracingPipelineHandle& texture, const GPURayTracingPipelineDescriptor& descriptor);
        void (*delete_raytracing_pipeline)(GPURayTracingPipelineHandle texture);

        bool (*create_command_buffer)(GPUQueueHandle queue, GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBufferDescriptor& descriptor);

        void (*cmd_set_render_pipeline)(GPUCommandEncoderHandle cmdbuffer, GPURenderPipelineHandle pipeline);
        void (*cmd_set_compute_pipeline)(GPUCommandEncoderHandle cmdbuffer, GPUComputePipelineHandle pipeline);
        void (*cmd_set_raytracing_pipeline)(GPUCommandEncoderHandle cmdbuffer, GPURayTracingPipelineHandle pipeline);
        void (*cmd_set_bind_group)(GPUCommandEncoderHandle cmdbuffer, GPUPipelineLayoutHandle layout, GPUIndex32 index, GPUBindGroupHandle bind_group, const Vector<GPUBufferDynamicOffset>& dynamic_offsets);
        void (*cmd_set_index_buffer)(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle buffer, GPUIndexFormat format, GPUSize64 offset, GPUSize64 size);
        void (*cmd_set_vertex_buffer)(GPUCommandEncoderHandle cmdbuffer, GPUIndex32 slot, GPUBufferHandle buffer, GPUSize64 offset, GPUSize64 size);
        void (*cmd_draw)(GPUCommandEncoderHandle cmdbuffer, GPUSize32 vertex_count, GPUSize32 instance_count, GPUSize32 first_vertex, GPUSize32 first_instance);
        void (*cmd_draw_indexed)(GPUCommandEncoderHandle cmdbuffer, GPUSize32 index_count, GPUSize32 instance_count, GPUSize32 first_index, GPUSignedOffset32 base_vertex, GPUSize32 first_instance);
        void (*cmd_draw_indirect)(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset);
        void (*cmd_draw_indexed_indirect)(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset);
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_API_H
