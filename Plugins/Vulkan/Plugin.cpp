// global module headers
#include <Common/String.h>
#include <Common/Plugin.h>
#include <Render/Render.hpp>
#include <Window/Window.hpp>

using namespace lyra;
using namespace lyra::rhi;
using namespace lyra::wsi;

auto get_api_name() -> CString { return "Vulkan"; }

LYRA_EXPORT auto prepare() -> void
{
    // vk_check(volkInitialize());

    // // initialize the mapping from render enums to vulkan enums for quick access
    // init_vulkan_mappings();
}

LYRA_EXPORT auto cleanup() -> void
{
}

LYRA_EXPORT auto create() -> RenderAPI
{
    auto api = RenderAPI{};
    // api.create_buffer           = create_buffer;
    // api.create_compute_pipeline = create_compute_pipeline;
    // api.create_debugger         = create_debugger;
    // api.create_device           = create_device;
    // api.create_fence            = create_fence;
    // api.create_instance         = create_instance;
    // api.create_semaphore        = create_semaphore;
    // api.create_shader           = create_shader;
    // api.create_surface          = create_surface;
    // api.create_swapchain        = create_swapchain;
    // api.create_texture          = create_texture;
    // api.create_texture_view     = create_texture_view;
    // api.destroy_buffer          = destroy_buffer;
    // api.destroy_debugger        = destroy_debugger;
    // api.destroy_device          = destroy_device;
    // api.destroy_fence           = destroy_fence;
    // api.destroy_instance        = destroy_instance;
    // api.destroy_pipeline        = destroy_pipeline;
    // api.destroy_semaphore       = destroy_semaphore;
    // api.destroy_shader          = destroy_shader;
    // api.destroy_surface         = destroy_surface;
    // api.destroy_swapchain       = destroy_swapchain;
    // api.destroy_texture         = destroy_texture;
    // api.destroy_texture_view    = destroy_texture_view;
    // api.get_api_name            = get_api_name;
    // api.get_adaptor_count       = get_adaptor_count;
    // api.get_adaptor_infos       = get_adaptor_infos;
    // api.get_buffer_desc         = get_buffer_desc;
    // api.get_texture_desc        = get_texture_desc;
    // api.acquire_frame           = acquire_frame;
    // api.present_frame           = present_frame;
    // api.get_back_buffers        = get_back_buffers;
    // api.wait_device             = wait_device;
    // api.wait_queue              = wait_queue;
    // api.wait_fences             = wait_fences;
    // api.reset_fences            = reset_fences;
    return api;
}
