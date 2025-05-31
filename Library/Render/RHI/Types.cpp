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

void GPUSurface::destroy()
{
    RHI::api()->delete_surface();
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

void GPUDevice::destroy()
{
    RHI::api()->delete_device();
}
