// global module headers
#include <Common/String.h>
#include <Common/Plugin.h>
#include <Render/Render.hpp>
#include <Window/Window.hpp>

#include "VkUtils.h"

using namespace lyra;
using namespace lyra::rhi;
using namespace lyra::wsi;

auto get_api_name() -> CString { return "Vulkan"; }

LYRA_EXPORT auto prepare() -> void
{
    vk_check(volkInitialize());
}

LYRA_EXPORT auto cleanup() -> void
{
}

LYRA_EXPORT auto create() -> RenderAPI
{
    auto api            = RenderAPI{};
    api.create_instance = create_instance;
    api.delete_instance = delete_instance;
    api.create_adapter  = create_adapter;
    api.delete_adapter  = delete_adapter;
    api.create_device   = create_device;
    api.delete_device   = delete_device;
    api.create_surface  = create_surface;
    api.delete_surface  = delete_surface;
    api.create_buffer   = create_buffer;
    api.delete_buffer   = delete_buffer;
    api.create_texture  = create_texture;
    api.delete_texture  = delete_texture;
    api.create_sampler  = create_sampler;
    api.delete_sampler  = delete_sampler;
    api.create_fence    = create_fence;
    api.delete_fence    = delete_fence;
    return api;
}
