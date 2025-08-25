#pragma once

#ifndef LYRA_LIBRARY_GUI_TYPES_H
#define LYRA_LIBRARY_GUI_TYPES_H

#include <imgui.h>

#include <Lyra/Common/Plugin.h>
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/GuiKit/GUIAPI.h>

namespace lyra
{
    using GUIRenderPlugin = Plugin<GUIRenderAPI>;

    struct GUI
    {
        // implicit conversion
        FORCE_INLINE GUI() : handle() {}
        FORCE_INLINE GUI(GUIHandle handle) : handle(handle) {}

        // implicit conversion
        FORCE_INLINE operator GUIHandle() { return handle; }
        FORCE_INLINE operator GUIHandle() const { return handle; }

        FORCE_INLINE static auto api() -> GUIRenderAPI*
        {
            return get_plugin()->get_api();
        }

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
        static auto get_plugin() -> Own<GUIRenderPlugin>&
        {
            static Own<GUIRenderPlugin> PLUGIN;
            return PLUGIN;
        }

        static void init_plugin()
        {
            auto& plugin = get_plugin();
            if (!plugin) {
                plugin = std::make_unique<GUIRenderPlugin>("lyra-imgui");
            }
        }

    private:
        GUIHandle handle;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_RENDER_GUI_TYPES_H
