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

    struct GUIRenderer
    {
        // implicit conversion
        FORCE_INLINE GUIRenderer() : handle() {}
        FORCE_INLINE GUIRenderer(GUIHandle handle) : handle(handle) {}

        // implicit conversion
        FORCE_INLINE operator GUIHandle() { return handle; }
        FORCE_INLINE operator GUIHandle() const { return handle; }

        FORCE_INLINE static auto api() -> GUIRenderAPI*
        {
            return get_plugin()->get_api();
        }

        FORCE_INLINE static auto init(const GUIDescriptor& descriptor) -> OwnedResource<GUIRenderer>
        {
            init_plugin();
            OwnedResource<GUIRenderer> gui(new GUIRenderer());
            GUIRenderer::api()->create_gui(gui->handle, descriptor);
            auto context = GUIRenderer::api()->get_context(gui->handle);
            ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(context));
            return gui;
        }

        FORCE_INLINE void destroy() const { GUIRenderer::api()->delete_gui(handle); }

        FORCE_INLINE void update() const { GUIRenderer::api()->update_gui(handle); }

        FORCE_INLINE void new_frame() const { GUIRenderer::api()->new_frame(handle); }

        FORCE_INLINE void end_frame() const { GUIRenderer::api()->end_frame(handle); }

        FORCE_INLINE void render_main_viewport(GPUCommandBuffer cmdbuffer, GPUTextureViewHandle backbuffer) const
        {
            GUIRenderer::api()->render_main_viewport(handle, cmdbuffer, backbuffer);
        }

        FORCE_INLINE void render_side_viewports() const
        {
            GUIRenderer::api()->render_side_viewports(handle);
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
