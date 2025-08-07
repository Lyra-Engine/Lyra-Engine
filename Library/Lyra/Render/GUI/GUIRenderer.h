#pragma once

#include "Lyra/Common/Slotmap.h"
#ifndef LYRA_LIBRARY_RENDER_GUI_RENDERER_H
#define LYRA_LIBRARY_RENDER_GUI_RENDERER_H

// imgui header(s)
#include <imgui.h>

#include <Lyra/Common/GLM.h>
#include <Lyra/Window/WSIAPI.h>
#include <Lyra/Window/WSITypes.h>
#include <Lyra/Render/RHI/RHIAPI.h>
#include <Lyra/Render/RHI/RHIAPI.h>
#include <Lyra/Render/SLC/SLCAPI.h>

namespace lyra::gui
{
    using namespace lyra::rhi;

    template <typename T>
    struct GUIGarbage
    {
        T    object;
        uint unused = 0;

        bool should_remove(uint threshold) { return unused++ >= threshold; }
    };

    struct GUIDescriptor
    {
        WindowHandle     window;  // primary window
        GPUSurfaceHandle surface; // primary surface/swapchain
        CompilerHandle   compiler;

        bool viewports = true;
    };

    struct GUITexture
    {
        GPUBindGroup   bindgroup;
        GPUTexture     texture;
        GPUTextureView view;

        bool valid() const { return texture.valid() || view.valid(); }
    };

    struct GUIWindowContext
    {
        WindowHandle    window;
        ImGuiContext*   context = nullptr;
        InputEventQuery events;
    };

    struct GUIPipelineData;
    struct GUIRendererData;
    struct GUIViewportData
    {
        GUIPipelineData* pipeline = nullptr;
        GUIRendererData* renderer = nullptr;
        GPUSurfaceHandle surface;
        WindowHandle     window;
        bool             owned;
    };

    struct GUIPlatformData
    {
        WindowHandle             primary_window;
        ImGuiContext*            context         = nullptr;
        float                    elapsed         = 0.0f;
        Vector<GUIWindowContext> window_contexts = {};
    };

    struct GUITextureDeleter
    {
        // deferred deletion, only reset the handles
        void operator()(GUITexture& texture)
        {
            texture.texture.handle.reset();
            texture.view.handle.reset();
        }
    };

    using GUIGarbageBuffer   = GUIGarbage<GPUBuffer>;
    using GUIGarbageBuffers  = Vector<GUIGarbageBuffer>;
    using GUIGarbageTexture  = GUIGarbage<GUITexture>;
    using GUIGarbageTextures = Vector<GUIGarbageTexture>;
    using GUITextureManager  = Slotmap<GUITexture, GUITextureDeleter>;

    struct GUIPipelineData
    {
        GPURenderPipeline                pipeline;
        GPUPipelineLayout                playout;
        Vector<GPUBindGroupLayoutHandle> blayouts;
        GPUShaderModule                  vshader;
        GPUShaderModule                  fshader;
        GPUSampler                       sampler;
        GUITextureManager                textures;
        GUIGarbageBuffers                garbage_buffers;
        GUIGarbageTextures               garbage_textures;
        uint                             frame_count = 3;
        uint                             frame_index = 0;
    };

    struct GUIRendererData
    {
        Vector<GPUBuffer> vbuffers;
        Vector<GPUBuffer> ibuffers;
    };

    struct GUIRenderer
    {
    public:
        static auto init(const GUIDescriptor& descriptor) -> OwnedResource<GUIRenderer>;

        void begin();
        void end();
        void prepare(GPUCommandBuffer cmdbuffer);
        void render(GPUCommandBuffer cmdbuffer, GPUSurfaceTexture backbuffer);
        void update();
        void destroy();

        // ImGuiContext* is initialized inside engine DLLs.
        // User application needs the same context in order to use ImGui.
        auto context() const -> ImGuiContext*;

    private:
        void init_imgui_setup(const GUIDescriptor& descriptor);
        void init_config_flags(const GUIDescriptor& descriptor);
        void init_backend_flags(const GUIDescriptor& descriptor);
        void init_platform_data(const GUIDescriptor& descriptor);
        void init_pipeline_data(const GUIDescriptor& descriptor);
        void init_renderer_data(const GUIDescriptor& descriptor);
        void init_viewport_data(const GUIDescriptor& descriptor);
        void init_dummy_texture();

        void begin_render_pass(GPUCommandBuffer cmdbuffer, GPUSurfaceTexture backbuffer);
        void setup_render_state(GPUCommandBuffer cmdbuffer, ImDrawData* draw_data, int width, int height);

        void update_key_state(ImGuiIO& io, const GUIWindowContext& ctx);
        void update_mouse_state(ImGuiIO& io, const GUIWindowContext& ctx);
        void update_viewport_state(ImGuiIO& io, const GUIWindowContext& ctx);
        void update_monitor_state();

        Own<GUIPlatformData> platform_data = nullptr;
        Own<GUIPipelineData> pipeline_data = nullptr;
        Own<GUIRendererData> renderer_data = nullptr;
        Vector<MonitorInfo>  monitors;
    };

} // namespace lyra::gui

#endif // LYRA_LIBRARY_RENDER_GUI_RENDERER_H
