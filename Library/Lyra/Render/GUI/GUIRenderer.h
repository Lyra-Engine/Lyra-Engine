#pragma once

#ifndef LYRA_LIBRARY_RENDER_GUI_RENDERER_H
#define LYRA_LIBRARY_RENDER_GUI_RENDERER_H

// imgui header(s)
#include <imgui.h>

#include <Lyra/Common/GLM.h>
#include <Lyra/Window/WSIAPI.h>
#include <Lyra/Render/RHI/RHIAPI.h>
#include <Lyra/Render/RHI/RHITypes.h>
#include <Lyra/Render/SLC/SLCTypes.h>

namespace lyra::gui
{
    using namespace lyra::rhi;

    struct GUIGarbage
    {
        GPUBuffer buffer;
        uint      unused_frames = 0;

        bool should_remove(uint threshold) { return unused_frames++ >= threshold; }
    };

    struct GUIDescriptor
    {
        WindowHandle window; // primary window

        bool viewports = true;
    };

    struct GUITexture
    {
        GPUBindGroup   bindgroup;
        GPUTexture     texture;
        GPUTextureView view;
    };

    struct GUIWindowContext
    {
        WindowHandle  window;
        ImGuiContext* context = nullptr;
    };

    struct GUIViewportData
    {
        WindowHandle window;
        bool         owned;
    };

    struct GUIPlatformData
    {
        WindowHandle             primary_window;
        ImGuiContext*            context         = nullptr;
        float                    elapsed         = 0.0f;
        Vector<GUIWindowContext> window_contexts = {};
    };

    struct GUIRendererData
    {
        GPURenderPipeline                pipeline;
        GPUPipelineLayout                playout;
        GPUShaderModule                  vshader;
        GPUShaderModule                  fshader;
        GPUBuffer                        transform;
        GPUBuffer                        vbuffer;
        GPUBuffer                        ibuffer;
        GPUSampler                       sampler;
        Vector<GPUBindGroupLayoutHandle> blayouts;
        List<GUITexture>                 textures;
        Vector<GUIGarbage>               garbages;
        uint                             image_count = 3;
    };

    struct GUIRenderer
    {
    public:
        static auto init(Compiler* compiler, const GUIDescriptor& descriptor) -> OwnedResource<GUIRenderer>;

        void reset(); // start of a new frame
        void render(GPUCommandBuffer cmdbuffer, ImDrawData* draw_data);
        void destroy();

    private:
        void init_backend_data(const GUIDescriptor& descriptor);
        void init_renderer_data(Compiler* compiler);
        void init_multi_viewport();

        void process_texture(GPUCommandBuffer cmdbuffer, ImTextureData* tex);
        void create_texture(ImTextureData* tex);
        void update_texture(GPUCommandBuffer cmdbuffer, ImTextureData* tex);
        void delete_texture(ImTextureData* tex);

        void create_texture_descriptors();
        void setup_render_state(GPUCommandBuffer cmdbuffer, int width, int height);

        void create_window_context(WindowHandle window, ImGuiContext* context);
        void delete_window_context(WindowHandle window);

        Own<GUIPlatformData> platform_data = nullptr;
        Own<GUIRendererData> renderer_data = nullptr;
    };

} // namespace lyra::gui

#endif // LYRA_LIBRARY_RENDER_GUI_RENDERER_H
