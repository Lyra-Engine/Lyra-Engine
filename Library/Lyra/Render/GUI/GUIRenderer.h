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
#include <Lyra/Render/RHI/RHITypes.h>
#include <Lyra/Render/SLC/SLCTypes.h>

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
        WindowHandle window; // primary window

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

    struct GUITextureDeleter
    {
        // doing nothing here, deferred deletion
        void operator()(GUITexture& texture) {}
    };

    using GUIGarbageBuffer   = GUIGarbage<GPUBuffer>;
    using GUIGarbageBuffers  = Vector<GUIGarbageBuffer>;
    using GUIGarbageTexture  = GUIGarbage<GUITexture>;
    using GUIGarbageTextures = Vector<GUIGarbageTexture>;
    using GUITextureManager  = Slotmap<GUITexture, GUITextureDeleter>;

    struct GUIRendererData
    {
        GPURenderPipeline                pipeline;
        GPUPipelineLayout                playout;
        GPUShaderModule                  vshader;
        GPUShaderModule                  fshader;
        GPUBuffer                        vbuffer;
        GPUBuffer                        ibuffer;
        GPUSampler                       sampler;
        Vector<GPUBindGroupLayoutHandle> blayouts;
        GUITextureManager                textures;
        GUIGarbageBuffers                garbage_buffers;
        GUIGarbageTextures               garbage_textures;
        uint                             image_count = 3;
    };

    struct GUIRenderer
    {
    public:
        static auto init(Compiler* compiler, const GUIDescriptor& descriptor) -> OwnedResource<GUIRenderer>;

        void reset(); // start of a new frame
        void render(GPUCommandBuffer cmdbuffer, GPUSurfaceTexture backbuffer, ImDrawData* draw_data);
        void update();
        void destroy();

        // ImGuiContext* is initialized inside engine DLLs.
        // User application needs the same context in order to use ImGui.
        auto context() const -> ImGuiContext*;

    private:
        void init_imgui_setup(const GUIDescriptor& descriptor);
        void init_platform_data(const GUIDescriptor& descriptor);
        void init_renderer_data(Compiler* compiler);
        void init_multi_viewport(const GUIDescriptor& descriptor);
        void init_config_flags(const GUIDescriptor& descriptor);
        void init_backend_flags(const GUIDescriptor& descriptor);
        void init_dummy_texture();

        void process_texture(GPUCommandBuffer cmdbuffer, ImTextureData* tex);
        void create_texture(ImTextureData* tex);
        void update_texture(GPUCommandBuffer cmdbuffer, ImTextureData* tex);
        void delete_texture(ImTextureData* tex);

        auto prepare_buffer(GPUBuffer buffer, uint size, GPUBufferUsageFlags usages) -> GPUBuffer;
        auto create_buffer(uint size, GPUBufferUsageFlags usages) -> GPUBuffer;

        void create_vertex_buffers(ImDrawData* draw_data);
        void create_texture_descriptors();
        void begin_render_pass(GPUCommandBuffer cmdbuffer, GPUSurfaceTexture backbuffer);
        void setup_render_state(GPUCommandBuffer cmdbuffer, ImDrawData* draw_data, int width, int height);

        void update_key_state(ImGuiIO& io, const GUIWindowContext& ctx);
        void update_mouse_state(ImGuiIO& io, const GUIWindowContext& ctx);

        void create_window_context(WindowHandle window, ImGuiContext* context);
        void delete_window_context(WindowHandle window);

        Own<GUIPlatformData> platform_data = nullptr;
        Own<GUIRendererData> renderer_data = nullptr;
    };

} // namespace lyra::gui

#endif // LYRA_LIBRARY_RENDER_GUI_RENDERER_H
