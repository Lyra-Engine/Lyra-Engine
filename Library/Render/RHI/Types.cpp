#include <Common/Plugin.h>
#include <Render/RHI/API.h>
#include <Render/RHI/Types.h>

using namespace lyra;
using namespace lyra::rhi;

using RenderPlugin = Plugin<RenderAPI>;

static Own<RenderPlugin> RENDER_PLUGIN;

RHI RHI::init(const RHIDescriptor& descriptor)
{
    if (!RENDER_PLUGIN) {
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
    }

    RHI rhi;
    // TODO: fill in the creation process here
    return rhi;
}
