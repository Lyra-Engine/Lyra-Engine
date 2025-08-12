#pragma once

#ifndef LYRA_LIBRARY_RENDER_GUI_API_H
#define LYRA_LIBRARY_RENDER_GUI_API_H

#include <Lyra/Render/RHI/RHITypes.h>
#include <Lyra/Render/SLC/SLCTypes.h>

namespace lyra::gui
{
    using namespace lyra::rhi;

    struct GUI;

    struct GUIDescriptor
    {
        WindowHandle     window;  // primary window
        GPUSurfaceHandle surface; // primary surface/swapchain
        CompilerHandle   compiler;

        // options
        bool docking   = true;
        bool viewports = true;
    };

    using GUIHandle = TypedPointerHandle<GUI>;

    struct GUIRenderAPI
    {
        // api name
        CString (*get_api_name)();

        bool (*create_gui)(GUIHandle& gui, const GUIDescriptor& descriptor);
        void (*delete_gui)(GUIHandle gui);
        void (*update_gui)(GUIHandle gui);

        void (*new_frame)(GUIHandle gui);
        void (*end_frame)(GUIHandle gui);

        void* (*get_context)(GUIHandle gui);

        void (*render_main_viewport)(GUIHandle gui, GPUCommandEncoderHandle cmdbuffer, GPUTextureViewHandle backbuffer);
        void (*render_side_viewports)(GUIHandle gui);
    };

} // namespace lyra::gui

#endif // LYRA_LIBRARY_RENDER_GUI_API_H
