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
    auto api                       = RenderAPI{};
    api.create_instance            = create_instance;
    api.delete_instance            = delete_instance;
    api.create_adapter             = create_adapter;
    api.delete_adapter             = delete_adapter;
    api.create_device              = create_device;
    api.delete_device              = delete_device;
    api.create_surface             = create_surface;
    api.delete_surface             = delete_surface;
    api.create_buffer              = create_buffer;
    api.delete_buffer              = delete_buffer;
    api.create_texture             = create_texture;
    api.delete_texture             = delete_texture;
    api.create_sampler             = create_sampler;
    api.delete_sampler             = delete_sampler;
    api.create_fence               = create_fence;
    api.delete_fence               = delete_fence;
    api.create_shader_module       = create_shader_module;
    api.delete_shader_module       = delete_shader_module;
    api.create_pipeline_layout     = create_pipeline_layout;
    api.delete_pipeline_layout     = delete_pipeline_layout;
    api.create_render_pipeline     = create_render_pipeline;
    api.delete_render_pipeline     = delete_render_pipeline;
    api.create_compute_pipeline    = create_compute_pipeline;
    api.delete_compute_pipeline    = delete_compute_pipeline;
    api.create_raytracing_pipeline = create_raytracing_pipeline;
    api.delete_raytracing_pipeline = delete_raytracing_pipeline;
    return api;
}
