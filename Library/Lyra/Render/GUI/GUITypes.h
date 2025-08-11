#pragma once

#ifndef LYRA_LIBRARY_RENDER_GUI_UTILS_H
#define LYRA_LIBRARY_RENDER_GUI_UTILS_H

#include <imgui.h>

#include <Lyra/Common/Plugin.h>
#include <Lyra/Render/GUI/GUIAPI.h>

namespace lyra::gui
{
    struct GUI
    {
        GUIHandle handle;

        // implicit conversion
        GUI() : handle() {}
        GUI(GUIHandle handle) : handle(handle) {}

        operator GUIHandle() { return handle; }
        operator GUIHandle() const { return handle; }

        static auto api() -> GUIRenderAPI*;

        FORCE_INLINE static auto init(const GUIDescriptor& descriptor) -> OwnedResource<GUI>
        {
            init_plugin();
            OwnedResource<GUI> gui(new GUI());
            GUI::api()->create_gui(gui->handle, descriptor);
            auto context = GUI::api()->get_context(gui->handle);
            ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(context));
            return gui;
        }

        FORCE_INLINE void destroy() const { GUI::api()->delete_gui(handle); }

        FORCE_INLINE void update() const { GUI::api()->update_gui(handle); }

        FORCE_INLINE void new_frame() const { GUI::api()->new_frame(handle); }

        FORCE_INLINE void end_frame() const { GUI::api()->end_frame(handle); }

        FORCE_INLINE void render_main_viewport(GPUCommandBuffer cmdbuffer, GPUTextureViewHandle backbuffer) const
        {
            GUI::api()->render_main_viewport(handle, cmdbuffer, backbuffer);
        }

        FORCE_INLINE void render_side_viewports() const
        {
            GUI::api()->render_side_viewports(handle);
        }

        template <typename F>
        FORCE_INLINE void ui(F&& f) const
        {
            new_frame();
            f();
            end_frame();
        }

    private:
        static void init_plugin();
    };

} // namespace lyra::gui

#endif // LYRA_LIBRARY_RENDER_GUI_UTILS_H
