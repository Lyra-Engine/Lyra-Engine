#include <Lyra/Engine/System/ImGuiManager.h>

using namespace lyra;

ImGuiManager::ImGuiManager(const GUIDescriptor& descriptor) : descriptor(descriptor)
{
    gui = GUIRenderer::init(descriptor);
}

void ImGuiManager::bind(Application& app)
{
    // save imgui manager into blackboard
    app.get_blackboard().add<GUIRenderer*>(gui.get());

    // bind imgui manager events
    app.bind<AppEvent::RESIZE>(&ImGuiManager::resize, this);
    app.bind<AppEvent::UPDATE>(&ImGuiManager::update, this);
    app.bind<AppEvent::UPDATE_PRE>(&ImGuiManager::pre_update, this);
    app.bind<AppEvent::UPDATE_POST>(&ImGuiManager::post_update, this);
    app.bind<AppEvent::RENDER_POST>(&ImGuiManager::render, this);
}

void ImGuiManager::update()
{
    gui->update();
}

void ImGuiManager::pre_update()
{
    gui->new_frame();
}

void ImGuiManager::post_update()
{
    gui->end_frame();
}

void ImGuiManager::render()
{
    if (descriptor.viewports)
        gui->render_side_viewports();
}

void ImGuiManager::resize()
{
    gui->resize();
}

ImGuiContext* ImGuiManager::context()
{
    return reinterpret_cast<ImGuiContext*>(gui->context());
}
