#ifndef LYRA_LIBRARY_RENDER_RHI_API_H
#define LYRA_LIBRARY_RENDER_RHI_API_H

// #include <Common/Plugin.h>

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

        // backend api
        GPUBackend (*get_backend)();

        bool (*create_adapter)(GPUAdapterHandle& adapter, const GPUAdapterDescriptor& descriptor);
        void (*delete_adapter)(GPUAdapterHandle adapter);

        bool (*create_device)(GPUAdapterHandle adapter, GPUDeviceHandle& device, const GPUDeviceDescriptor& descriptor);
        void (*delete_device)(GPUDeviceHandle device);

        bool (*create_surface)(GPUDeviceHandle device, GPUSurfaceHandle& surface, const GPUSurfaceDescriptor& descriptor);
        void (*delete_surface)(GPUDeviceHandle device, GPUSurfaceHandle surface);

        bool (*create_buffer)(GPUDeviceHandle device, GPUBufferHandle& buffer, const GPUBufferDescriptor& descriptor);
        void (*delete_buffer)(GPUDeviceHandle device, GPUBufferHandle buffer);

        bool (*create_sampler)(GPUDeviceHandle device, GPUSamplerHandle& sampler, const GPUSamplerDescriptor& descriptor);
        void (*delete_sampler)(GPUDeviceHandle device, GPUSamplerHandle sampler);

        bool (*create_texture)(GPUDeviceHandle device, GPUTextureHandle& texture, const GPUTextureDescriptor& descriptor);
        void (*delete_texture)(GPUDeviceHandle device, GPUTextureHandle texture);

        bool (*create_texture_view)(GPUDeviceHandle device, GPUTextureHandle texture, GPUTextureViewHandle& view, const GPUTextureViewDescriptor& descriptor);
        void (*delete_texture_view)(GPUDeviceHandle device, GPUTextureHandle texture, GPUTextureViewHandle view);

        bool (*create_shader_module)(GPUDeviceHandle device, GPUShaderModuleHandle& texture, const GPUShaderModuleDescriptor& descriptor);
        void (*delete_shader_module)(GPUDeviceHandle device, GPUShaderModuleHandle texture);

        bool (*create_blas)(GPUDeviceHandle device, GPUBlasHandle& texture, const GPUBlasDescriptor& descriptor);
        void (*delete_blas)(GPUDeviceHandle device, GPUBlasHandle texture);

        bool (*create_tlas)(GPUDeviceHandle device, GPUTlasHandle& texture, const GPUTlasDescriptor& descriptor);
        void (*delete_tlas)(GPUDeviceHandle device, GPUTlasHandle texture);

        bool (*create_render_pipeline)(GPUDeviceHandle device, GPURenderPipelineHandle& texture, const GPURenderPipelineDescriptor& descriptor);
        void (*delete_render_pipeline)(GPUDeviceHandle device, GPURenderPipelineHandle texture);

        bool (*create_compute_pipeline)(GPUDeviceHandle device, GPUComputePipelineHandle& texture, const GPUComputePipelineDescriptor& descriptor);
        void (*delete_compute_pipeline)(GPUDeviceHandle device, GPUComputePipelineHandle texture);

        bool (*create_raytracing_pipeline)(GPUDeviceHandle device, GPURayTracingPipelineHandle& texture, const GPURayTracingPipelineDescriptor& descriptor);
        void (*delete_raytracing_pipeline)(GPUDeviceHandle device, GPURayTracingPipelineHandle texture);

        bool (*create_command_buffer)(GPUDeviceHandle device, GPUQueueHandle queue, GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBufferDescriptor& descriptor);

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

    // using RenderPlugin = Plugin<RenderAPI>;

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_API_H
