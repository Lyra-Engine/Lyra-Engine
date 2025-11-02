#pragma once

#ifndef LYRA_LIBRARY_EDITOR_GUI_MANAGER_H
#define LYRA_LIBRARY_EDITOR_GUI_MANAGER_H

#include <Lyra/GuiKit/GUITypes.h>
#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    // GUIManager is only a wrapper around GUIRenderer.
    // It is created to used with AppKit's style of data
    // handling and event binding.
    struct GUIManager
    {
    public:
        explicit GUIManager(const GUIDescriptor& descriptor);

        void bind(Application& app);

        void update();

        void pre_update();

        void post_update();

        void render();

        void resize();

        auto context() -> ImGuiContext*;

        // apply_context() is required to be called from user application
        // because currently we created GUIRenderer in Lyra-Engine.dll.
        // User application will need ImGuiContext* if it intends to call
        // any ImGui functions.
        FORCE_INLINE void apply_context()
        {
            gui->apply_context();
        }

    private:
        GUIDescriptor              descriptor;
        OwnedResource<GUIRenderer> gui;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_EDITOR_GUI_MANAGER_H
