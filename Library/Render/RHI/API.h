#ifndef LYRA_LIBRARY_RENDER_RHI_API_H
#define LYRA_LIBRARY_RENDER_RHI_API_H

// #include <Common/Plugin.h>

#include <Render/RHI/Enums.h>
#include <Render/RHI/Utils.h>
#include <Render/RHI/Descs.h>
#include <Render/RHI/Mixin.h>
#include <Render/RHI/Error.h>
#include <Render/RHI/Types.h>

namespace lyra::rhi
{
    struct RenderAPI
    {
        // backend api
        GPUBackend (*get_backend)();

        bool (*create_adapter)(GPUAdapterHandle& adapter, const GPUAdapterDescriptor& descriptor);
        void (*delete_adapter)(GPUAdapterHandle adapter);

        bool (*create_device)(GPUAdapterHandle adapter, GPUDeviceHandle& device, const GPUDeviceDescriptor& descriptor);
        void (*delete_device)(GPUDeviceHandle device);

        bool (*create_swapchain)(GPUDeviceHandle device, GPUSwapchainHandle& buffer, const GPUSwapchainDescriptor& descriptor);
        void (*delete_swapchain)(GPUDeviceHandle device, GPUSwapchainHandle buffer);

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

        bool (*create_command_buffer)();
    };

    // using RenderPlugin = Plugin<RenderAPI>;

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_API_H
