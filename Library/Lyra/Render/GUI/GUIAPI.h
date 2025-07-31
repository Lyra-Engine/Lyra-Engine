#pragma once

#ifndef LYRA_LIBRARY_RENDER_GUI_API_H
#define LYRA_LIBRARY_RENDER_GUI_API_H

// imgui header(s)
#include <imgui.h>

#include <Lyra/Render/RHI/RHIAPI.h>
#include <Lyra/Render/RHI/RHIUtils.h>

namespace lyra::gui
{
    using namespace lyra::rhi;

    struct GUIAPI
    {
        // used by imgui's multi-viewport docking support
        void (*create_window)(ImGuiViewport* viewport);
        void (*delete_window)(ImGuiViewport* viewport);
        void (*show_window)(ImGuiViewport* viewport);
        void (*update_window)(ImGuiViewport* viewport, const WindowInputState& state);
        void (*render_window)(ImGuiViewport* viewport, void*);
        void (*swap_buffers)(ImGuiViewport* viewport, void*);
        void (*set_window_pos)(ImGuiViewport* viewport, ImVec2 pos);
        auto (*get_window_pos)(ImGuiViewport* viewport) -> ImVec2;
        void (*set_window_size)(ImGuiViewport* viewport, ImVec2 size);
        auto (*get_window_size)(ImGuiViewport* viewport) -> ImVec2;
        auto (*get_window_scale)(ImGuiViewport* viewport) -> ImVec2;
        void (*set_window_title)(ImGuiViewport* viewport, CString title);
        void (*set_window_focus)(ImGuiViewport* viewport);
        bool (*get_window_focus)(ImGuiViewport* viewport);
        bool (*get_window_minimized)(ImGuiViewport* viewport);
        void (*set_window_alpha)(ImGuiViewport* viewport, float alpha);

        // used to render imgui draw data
        void (*newframe)();
        void (*render)(GPUCommandEncoderHandle cmdbuffer, ImDrawData* draw_data);
    };

} // namespace lyra::gui

#endif // LYRA_LIBRARY_RENDER_GUI_API_H
