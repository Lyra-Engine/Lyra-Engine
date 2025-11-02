#include <Lyra/Editor/GUIManager.h>

using namespace lyra;

GUIManager::GUIManager(const GUIDescriptor& descriptor) : descriptor(descriptor)
{
    gui = GUIRenderer::init(descriptor);
}

void GUIManager::bind(Application& app)
{
    // save imgui manager into blackboard
    app.get_blackboard().add<GUIRenderer*>(gui.get());

    // bind imgui manager events
    app.bind<AppEvent::RESIZE>(&GUIManager::resize, this);
    app.bind<AppEvent::UPDATE>(&GUIManager::update, this);
    app.bind<AppEvent::UPDATE_PRE>(&GUIManager::pre_update, this);
    app.bind<AppEvent::UPDATE_POST>(&GUIManager::post_update, this);
    app.bind<AppEvent::RENDER_POST>(&GUIManager::render, this);
}

void GUIManager::update()
{
    gui->update();
}

void GUIManager::pre_update()
{
    gui->new_frame();
}

void GUIManager::post_update()
{
    gui->end_frame();
}

void GUIManager::render()
{
    if (descriptor.viewports)
        gui->render_side_viewports();
}

void GUIManager::resize()
{
    gui->resize();
}

ImGuiContext* GUIManager::context()
{
    return reinterpret_cast<ImGuiContext*>(gui->context());
}
