#include <Common/Plugin.h>
#include <Render/RHI/API.h>
#include <Render/RHI/Types.h>

using namespace lyra;
using namespace lyra::rhi;

using RenderPlugin = Plugin<RenderAPI>;

static Own<RenderPlugin> RENDER_PLUGIN;

// global objects
GPUDevice  RHI::DEVICE  = {};
GPUSurface RHI::SURFACE = {};

OwnedResource<RHI> RHI::init(const RHIDescriptor& descriptor)
{
    if (RENDER_PLUGIN.get()) {
        show_error("RHI", "Call RHI::init() exactly once!");
        exit(1);
    }

    switch (descriptor.backend) {
        case RHIBackend::D3D12:
            RENDER_PLUGIN = std::make_unique<RenderPlugin>("lyra-d3d12");
            break;
        case RHIBackend::METAL:
            RENDER_PLUGIN = std::make_unique<RenderPlugin>("lyra-metal");
            break;
        case RHIBackend::VULKAN:
            RENDER_PLUGIN = std::make_unique<RenderPlugin>("lyra-vulkan");
            break;
    }

    OwnedResource<RHI> rhi(new RHI());
    rhi->flags   = descriptor.flags;
    rhi->backend = descriptor.backend;
    rhi->window  = descriptor.window;
    RHI::api()->create_instance(descriptor);
    return rhi;
}

RenderAPI* RHI::api()
{
    return RENDER_PLUGIN->get_api();
}

void RHI::destroy()
{
    RHI::api()->delete_surface();
    RHI::api()->delete_device();
    RHI::api()->delete_instance();
}

GPUAdapter RHI::request_adapter(const GPUAdapterDescriptor& descriptor)
{
    GPUAdapter adapter = {};
    RHI::api()->create_adapter(adapter, descriptor);
    return adapter;
}

GPUSurface RHI::request_surface(const GPUSurfaceDescriptor& descriptor)
{
    RHI::SURFACE = {};
    RHI::api()->create_surface(SURFACE, descriptor);
    return RHI::SURFACE;
}

GPUDevice GPUAdapter::request_device(const GPUDeviceDescriptor& descriptor)
{
    RHI::DEVICE.adapter_info = info;
    RHI::DEVICE.features     = features;
    RHI::api()->create_device(descriptor);

    // TODO: create queues

    return RHI::DEVICE;
}

GPUSurfaceTexture GPUSurface::get_current_texture()
{
    GPUSurfaceTexture texture;
    RHI::api()->acquire_next_frame(texture.handle, texture.available, texture.complete, texture.suboptimal);
    return texture;
}

void GPUSurface::destroy()
{
    RHI::api()->delete_surface();
}

void GPUSurfaceTexture::present()
{
    RHI::api()->present_curr_frame(handle);
}

GPUFence GPUDevice::create_fence()
{
    GPUFence fence;
    RHI::api()->create_fence(fence.handle);
    return fence;
}

GPUBuffer GPUDevice::create_buffer(const GPUBufferDescriptor& desc)
{
    GPUBuffer buffer;
    RHI::api()->create_buffer(buffer.handle, desc);
    return buffer;
}

GPUTexture GPUDevice::create_texture(const GPUTextureDescriptor& desc)
{
    GPUTexture texture;
    RHI::api()->create_texture(texture.handle, desc);
    return texture;
}

GPUSampler GPUDevice::create_sampler(const GPUSamplerDescriptor& desc)
{
    GPUSampler sampler;
    RHI::api()->create_sampler(sampler.handle, desc);
    return sampler;
}

GPUShaderModule GPUDevice::create_shader_module(const GPUShaderModuleDescriptor& desc)
{
    GPUShaderModule module;
    RHI::api()->create_shader_module(module.handle, desc);
    return module;
}

GPUQuerySet GPUDevice::create_query_set(const GPUQuerySetDescriptor& desc)
{
    GPUQuerySet query;
    RHI::api()->create_query_set(query.handle, desc);
    return query;
}

GPUBindGroup GPUDevice::create_bind_group(const GPUBindGroupDescriptor& desc)
{
    assert(!!!"unimplemented");
    return GPUBindGroup();
}

GPUBindGroupLayout GPUDevice::create_bind_group_layout(const GPUBindGroupLayoutDescriptor& desc)
{
    GPUBindGroupLayout layout;
    RHI::api()->create_bind_group_layout(layout.handle, desc);
    return layout;
}

GPUPipelineLayout GPUDevice::create_pipeline_layout(const GPUPipelineLayoutDescriptor& desc)
{
    GPUPipelineLayout layout;
    RHI::api()->create_pipeline_layout(layout.handle, desc);
    return layout;
}

GPURenderPipeline GPUDevice::create_render_pipeline(const GPURenderPipelineDescriptor& desc)
{
    GPURenderPipeline layout;
    RHI::api()->create_render_pipeline(layout.handle, desc);
    return layout;
}

GPUComputePipeline GPUDevice::create_compute_pipeline(const GPUComputePipelineDescriptor& desc)
{
    GPUComputePipeline layout;
    RHI::api()->create_compute_pipeline(layout.handle, desc);
    return layout;
}

GPURayTracingPipeline GPUDevice::create_raytracing_pipeline(const GPURayTracingPipelineDescriptor& desc)
{
    GPURayTracingPipeline layout;
    RHI::api()->create_raytracing_pipeline(layout.handle, desc);
    return layout;
}

GPUCommandBuffer GPUDevice::create_command_buffer(const GPUCommandBufferDescriptor& desc)
{
    GPUCommandBuffer command_buffer;
    RHI::api()->create_command_buffer(command_buffer.handle, desc);
    return command_buffer;
}

GPUCommandBundle GPUDevice::create_command_bundle(const GPUCommandBundleDescriptor& desc)
{
    GPUCommandBundle command_bundle;
    RHI::api()->create_command_bundle(command_bundle.handle, desc);
    return command_bundle;
}

void GPUDevice::wait()
{
    RHI::api()->wait_idle();
}

void wait(GPUFence fence)
{
    RHI::api()->wait_fence(fence.handle);
}

void GPUDevice::destroy()
{
    RHI::api()->delete_device();
}

void GPUBuffer::destroy()
{
    RHI::api()->delete_buffer(handle);
}

void GPUTexture::destroy()
{
    RHI::api()->delete_texture(handle);
}

void GPUShaderModule::destroy()
{
    RHI::api()->delete_shader_module(handle);
}

void GPURenderPipeline::destroy()
{
    RHI::api()->delete_render_pipeline(handle);
}

void GPUComputePipeline::destroy()
{
    RHI::api()->delete_compute_pipeline(handle);
}

void GPURayTracingPipeline::destroy()
{
    RHI::api()->delete_raytracing_pipeline(handle);
}

void GPUPipelineLayout::destroy()
{
    RHI::api()->delete_pipeline_layout(handle);
}
