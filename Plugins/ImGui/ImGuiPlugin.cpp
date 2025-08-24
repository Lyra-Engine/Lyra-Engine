// global module headers
#include <Lyra/Common/String.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/GuiKit/GUIAPI.h>

// local plugin header(s)
#include "GUIRenderer.h"

using namespace lyra;

static auto get_api_name() -> CString { return "ImGui"; }

static bool create_gui(GUIHandle& gui, const GUIDescriptor& descriptor)
{
    auto renderer = std::make_unique<GUIRenderer>(descriptor);
    gui.handle    = renderer.release();
    return true;
}

static void delete_gui(GUIHandle gui)
{
    auto renderer = gui.astype<GUIRenderer>();
    renderer->destroy();
    delete renderer;
}

static void new_frame(GUIHandle gui)
{
    gui.astype<GUIRenderer>()->new_frame();
}

static void end_frame(GUIHandle gui)
{
    gui.astype<GUIRenderer>()->end_frame();
}

static void update_gui(GUIHandle gui)
{
    gui.astype<GUIRenderer>()->update();
}

static void* get_context(GUIHandle gui)
{
    return gui.astype<GUIRenderer>()->context();
}

static void render_main_viewport(GUIHandle gui, GPUCommandEncoderHandle encoder, GPUTextureViewHandle backbuffer)
{
    auto cmdbuffer = GPUCommandBuffer(encoder);
    auto renderer  = gui.astype<GUIRenderer>();
    renderer->reset();
    renderer->prepare(cmdbuffer);
    renderer->begin_render_pass(cmdbuffer, backbuffer);
    renderer->render(cmdbuffer, backbuffer);
    renderer->end_render_pass(cmdbuffer);
}

static void render_side_viewports(GUIHandle gui)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

LYRA_EXPORT auto prepare() -> void
{
}

LYRA_EXPORT auto cleanup() -> void
{
}

LYRA_EXPORT auto create() -> GUIRenderAPI
{
    auto api                  = GUIRenderAPI{};
    api.get_api_name          = get_api_name;
    api.create_gui            = create_gui;
    api.delete_gui            = delete_gui;
    api.update_gui            = update_gui;
    api.new_frame             = new_frame;
    api.end_frame             = end_frame;
    api.get_context           = get_context;
    api.render_main_viewport  = render_main_viewport;
    api.render_side_viewports = render_side_viewports;
    return api;
}
