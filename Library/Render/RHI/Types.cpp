#include <Common/Plugin.h>
#include <Render/RHI/API.h>
#include <Render/RHI/Types.h>

using namespace lyra;
using namespace lyra::rhi;

using RenderPlugin = Plugin<RenderAPI>;

static Own<RenderPlugin> RENDER_PLUGIN;

#define RHIAPI (RENDER_PLUGIN->get_api())

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
    RHIAPI->create_instance(descriptor);
    return rhi;
}

void RHI::destroy()
{
    RHIAPI->delete_instance();
}

GPUAdapter RHI::request_adapter(const GPUAdapterDescriptor& descriptor)
{
    GPUAdapter adapter = {};
    RHIAPI->create_adapter(adapter, descriptor);
    return adapter;
}

GPUSurface RHI::request_surface(const GPUSurfaceDescriptor& descriptor)
{
    GPUSurface surface = {};
    RHIAPI->create_surface(surface, descriptor);
    return surface;
}

GPUDevice GPUAdapter::request_device(const GPUDeviceDescriptor& descriptor)
{
    GPUDevice device    = {};
    device.adapter_info = info;
    device.features     = features;
    RHIAPI->create_device(descriptor);
    // TODO: create queues
    return device;
}

void GPUSurface::destroy()
{
    RHIAPI->delete_surface();
}

void GPUDevice::destroy()
{
    RHIAPI->delete_device();
}
