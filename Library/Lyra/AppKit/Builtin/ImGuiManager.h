#pragma once

#ifndef LYRA_LIBRARY_APPKIT_IMGUI_MANAGER_H
#define LYRA_LIBRARY_APPKIT_IMGUI_MANAGER_H

#include <Lyra/GuiKit/GUITypes.h>
#include <Lyra/Appkit/AppTypes.h>

namespace lyra
{
    struct ImGuiManager
    {
    public:
        explicit ImGuiManager(const GUIDescriptor& descriptor) : descriptor(descriptor)
        {
            gui = GUIRenderer::init(descriptor);
        }

        void bind(Application& app)
        {
            // save imgui manager into blackboard
            app.get_blackboard().add<GUIRenderer*>(gui.get());

            // bind imgui manager events
            app.bind<AppEvent::UPDATE>(&ImGuiManager::update, this);
            app.bind<AppEvent::UPDATE_PRE>(&ImGuiManager::pre_update, this);
            app.bind<AppEvent::UPDATE_POST>(&ImGuiManager::post_update, this);
            app.bind<AppEvent::RENDER_POST>(&ImGuiManager::render, this);
        }

        void update()
        {
            gui->update();
        }

        void pre_update()
        {
            gui->new_frame();
        }

        void post_update()
        {
            gui->end_frame();
        }

        void render()
        {
            if (descriptor.viewports)
                gui->render_side_viewports();
        }

    private:
        GUIDescriptor              descriptor;
        OwnedResource<GUIRenderer> gui;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_APPKIT_IMGUI_MANAGER_H
