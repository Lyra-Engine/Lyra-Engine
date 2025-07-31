#include <Lyra/Common/Plugin.h>
#include <Lyra/Common/Pointer.h>
#include <Lyra/Common/Function.h>
#include <Lyra/Render/GUI/GUIRenderer.h>

using namespace lyra;
using namespace lyra::gui;

static const char* imgui_shader_source = R"""(
struct VertexInput
{
    float2 pos   : POSITION;
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

struct VertexOutput
{
    float4 pos   : SV_Position;
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

struct TextureInput
{
    Texture2D<float4> tex;
    SamplerState      smp;
};

ParameterBlock<float4x4>     proj;
ParameterBlock<TextureInput> tinp;

[shader("vertex")]
VertexOutput vsmain(VertexInput input)
{
    VertexOutput output;
    output.pos   = mul(float4(input.position, 0.0, 1.0), proj);
    output.uv    = input.uv;
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 fsmain(VertexOutput input) : SV_Target
{
    return tinp.tex.Sample(tinp.smp, input.uv) * input.color;
}
)""";

#pragma region GUIRenderer
OwnedResource<GUIRenderer> GUIRenderer::init(Compiler* compiler, const GUIDescriptor& descriptor)
{
    OwnedResource<GUIRenderer> gui(new GUIRenderer());
    gui->init_backend_data(descriptor);
    gui->init_renderer_data(compiler);
    return gui;
}

void GUIRenderer::reset()
{
    assert(platform_data && "ImGui platform data has not been initialized!");
    assert(renderer_data && "ImGui renderer data has not been initialized!");
}

void GUIRenderer::render(GPUCommandBuffer cmdbuffer, ImDrawData* draw_data)
{
    // avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width  = static_cast<int>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = static_cast<int>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0) return;

    // catch up with texture updates. Most of the times, the list will have 1 element with an OK status, aka nothing to do.
    // (This almost always points to ImGui::GetPlatformIO().Textures[] but is part of ImDrawData to allow overriding or disabling texture updates).
    if (draw_data->Textures != nullptr)
        for (ImTextureData* tex : *draw_data->Textures)
            if (tex->Status != ImTextureStatus_OK)
                update_texture(cmdbuffer, tex);

    // refresh all texture descriptors (because bind group / descriptors only have a life time within a frame)
    create_texture_descriptors();
}

void GUIRenderer::destroy()
{
    RHI::api()->wait_idle();

    ImGui::DestroyPlatformWindows();

    ImGuiIO& io                = ImGui::GetIO();
    io.BackendPlatformName     = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendFlags            = 0;
    delete_window_context(platform_data->primary_window);
}

void GUIRenderer::init_backend_data(const GUIDescriptor& descriptor)
{
    ImGuiIO& io = ImGui::GetIO();
    IMGUI_CHECKVERSION();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    platform_data                 = std::make_unique<GUIPlatformData>();
    platform_data->primary_window = descriptor.window;
    platform_data->context        = ImGui::GetCurrentContext();
    platform_data->elapsed        = 0.0f;
    IM_ASSERT(platform_data->primary_window.window == nullptr && "Expect a valid handle for primary window!");

    io.BackendPlatformUserData = platform_data.get();
    io.BackendPlatformName     = "lyra-window";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)

    if (descriptor.viewports) {
        init_multi_viewport();
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
    }

    ImGuiContext* context = ImGui::GetCurrentContext();
    create_window_context(descriptor.window, context);
}

void GUIRenderer::init_renderer_data(Compiler* compiler)
{
    renderer_data = std::make_unique<GUIRendererData>();

    auto& device = RHI::get_current_device();

    // shader module
    auto module = execute([&]() {
        auto desc   = CompileDescriptor{};
        desc.module = "imgui";
        desc.path   = "imgui.slang";
        desc.source = imgui_shader_source;
        return compiler->compile(desc);
    });

    // shader reflection
    auto refl = compiler->reflect({
        {*module, "vsmain"},
        {*module, "fsmain"},
    });

    // vertex shader
    renderer_data->vshader = execute([&]() {
        auto code  = module->get_shader_blob("vsmain");
        auto desc  = GPUShaderModuleDescriptor{};
        desc.label = "imgui_vertex_shader";
        desc.data  = code->data;
        desc.size  = code->size;
        return device.create_shader_module(desc);
    });

    // fragment shader
    renderer_data->fshader = execute([&]() {
        auto code  = module->get_shader_blob("fsmain");
        auto desc  = GPUShaderModuleDescriptor{};
        desc.label = "imgui_fragment_shader";
        desc.data  = code->data;
        desc.size  = code->size;
        return device.create_shader_module(desc);
    });

    // bind group layouts
    for (auto& layout : refl->get_bind_group_layouts()) {
        auto blayout = device.create_bind_group_layout(layout);
        renderer_data->blayouts.push_back(blayout.handle);
    }

    // pipelie layout
    renderer_data->playout = execute([&]() {
        auto desc               = GPUPipelineLayoutDescriptor{};
        desc.label              = "imgui_layout";
        desc.bind_group_layouts = renderer_data->blayouts;
        return device.create_pipeline_layout(desc);
    });

    // pipeline layout
    renderer_data->pipeline = execute([&]() {
        auto attributes = refl->get_vertex_attributes({
            {"pos", offsetof(ImDrawVert, pos)},
            {"uv", offsetof(ImDrawVert, uv)},
            {"color", offsetof(ImDrawVert, col)},
        });

        // slight modification to color vertex attribute
        attributes.at(2).format = GPUVertexFormat::UINT8x4;

        // vertex buffer layout
        GPUVertexBufferLayout buffer{};
        buffer.attributes   = attributes;
        buffer.array_stride = sizeof(ImDrawVert);
        buffer.step_mode    = GPUVertexStepMode::VERTEX;

        // color target
        GPUColorTargetState color_state{};
        color_state.format                 = GPUTextureFormat::RGBA8UNORM_SRGB;
        color_state.blend_enable           = true;
        color_state.blend.color.operation  = GPUBlendOperation::ADD;
        color_state.blend.color.src_factor = GPUBlendFactor::SRC_ALPHA;
        color_state.blend.color.dst_factor = GPUBlendFactor::ONE_MINUS_SRC_ALPHA;
        color_state.blend.alpha.operation  = GPUBlendOperation::ADD;
        color_state.blend.alpha.src_factor = GPUBlendFactor::SRC_ALPHA;
        color_state.blend.alpha.dst_factor = GPUBlendFactor::ONE_MINUS_SRC_ALPHA;
        color_state.write_mask             = GPUColorWrite::ALL;

        // pipeline descriptor
        GPURenderPipelineDescriptor desc{};
        desc.label                = "imgui_pipeline";
        desc.layout               = renderer_data->playout;
        desc.vertex.buffers       = buffer;
        desc.vertex.module        = renderer_data->vshader;
        desc.fragment.module      = renderer_data->fshader;
        desc.fragment.targets     = color_state;
        desc.multisample.count    = 1;
        desc.primitive.cull_mode  = GPUCullMode::BACK;
        desc.primitive.front_face = GPUFrontFace::CCW;
        desc.primitive.topology   = GPUPrimitiveTopology::TRIANGLE_LIST;

        return device.create_render_pipeline(desc);
    });

    // sampler
    renderer_data->sampler = execute([&]() {
        GPUSamplerDescriptor desc{};
        desc.label          = "imgui_sampler";
        desc.address_mode_u = GPUAddressMode::CLAMP_TO_EDGE;
        desc.address_mode_v = GPUAddressMode::CLAMP_TO_EDGE;
        desc.address_mode_w = GPUAddressMode::CLAMP_TO_EDGE;
        desc.lod_min_clamp  = 1.0;
        desc.lod_max_clamp  = 1.0;
        desc.compare_enable = false;
        desc.min_filter     = GPUFilterMode::LINEAR;
        desc.mag_filter     = GPUFilterMode::LINEAR;
        desc.mipmap_filter  = GPUMipmapFilterMode::LINEAR;
        desc.max_anisotropy = 1.0;
        return device.create_sampler(desc);
    });

    // buffer
    renderer_data->transform = execute([&]() {
        GPUBufferDescriptor desc{};
        desc.label              = "imgui_buffer";
        desc.size               = sizeof(glm::mat4x4);
        desc.usage              = GPUBufferUsage::UNIFORM | GPUBufferUsage::MAP_WRITE;
        desc.mapped_at_creation = true;
        return device.create_buffer(desc);
    });
}

void GUIRenderer::init_multi_viewport()
{
    // ImGuiPlatformIO& platform_io                   = ImGui::GetPlatformIO();
    // platform_io.Platform_CreateWindow              = GUIRenderer::api()->create_window;
    // platform_io.Platform_DestroyWindow             = GUIRenderer::api()->delete_window;
    // platform_io.Platform_ShowWindow                = GUIRenderer::api()->show_window;
    // platform_io.Platform_SetWindowPos              = GUIRenderer::api()->set_window_pos;
    // platform_io.Platform_GetWindowPos              = GUIRenderer::api()->get_window_pos;
    // platform_io.Platform_SetWindowSize             = GUIRenderer::api()->set_window_size;
    // platform_io.Platform_GetWindowSize             = GUIRenderer::api()->get_window_size;
    // platform_io.Platform_GetWindowFramebufferScale = GUIRenderer::api()->get_window_scale;
    // platform_io.Platform_SetWindowFocus            = GUIRenderer::api()->set_window_focus;
    // platform_io.Platform_GetWindowFocus            = GUIRenderer::api()->get_window_focus;
    // platform_io.Platform_GetWindowMinimized        = GUIRenderer::api()->get_window_minimized;
    // platform_io.Platform_SetWindowTitle            = GUIRenderer::api()->set_window_title;
    // platform_io.Platform_RenderWindow              = GUIRenderer::api()->render_window;
    // platform_io.Platform_SwapBuffers               = GUIRenderer::api()->swap_buffers;
    // platform_io.Platform_SetWindowAlpha            = GUIRenderer::api()->set_window_alpha;

    // register main window handle (which is owned by the main application, not by us)
    GUIViewportData* vd = new GUIViewportData{};
    vd->window          = platform_data->primary_window;
    vd->owned           = false;

    ImGuiViewport* viewport     = ImGui::GetMainViewport();
    viewport->PlatformUserData  = vd;
    viewport->PlatformHandleRaw = vd->window.window;
}

void GUIRenderer::process_texture(GPUCommandBuffer cmdbuffer, ImTextureData* tex)
{
    // nothing to do
    if (tex->Status == ImTextureStatus_OK) return;

    // create texture if necessary
    if (tex->Status == ImTextureStatus_WantCreate)
        create_texture(tex);

    // update texture if necessary
    if (tex->Status == ImTextureStatus_WantCreate || tex->Status == ImTextureStatus_WantUpdates)
        update_texture(cmdbuffer, tex);

    // delete texture if necessary
    if (tex->Status == ImTextureStatus_WantDestroy && tex->UnusedFrames >= renderer_data->image_count)
        delete_texture(tex);
}

void GUIRenderer::create_texture(ImTextureData* tex)
{
    auto& device = RHI::get_current_device();

    auto texture = execute([&]() {
        GPUTextureDescriptor desc{};
        desc.dimension       = GPUTextureDimension::x2D;
        desc.format          = GPUTextureFormat::RGBA8UNORM;
        desc.size.width      = tex->Width;
        desc.size.height     = tex->Height;
        desc.size.depth      = 1;
        desc.array_layers    = 1;
        desc.mip_level_count = 1;
        desc.sample_count    = 1;
        desc.usage           = GPUTextureUsage::COPY_DST | GPUTextureUsage::TEXTURE_BINDING;
        return device.create_texture(desc);
    });

    GUITexture texinfo{};
    texinfo.texture = texture;
    texinfo.view    = texture.create_view();

    renderer_data->textures.push_front(texinfo);
    tex->BackendUserData = &renderer_data->textures.front();
}

void GUIRenderer::update_texture(GPUCommandBuffer cmdbuffer, ImTextureData* tex)
{
}

void GUIRenderer::delete_texture(ImTextureData* tex)
{
}

void GUIRenderer::create_texture_descriptors()
{
    auto& device = RHI::get_current_device();

    for (auto& texinfo : renderer_data->textures) {
        Array<GPUBindGroupEntry, 2> entries;

        entries.at(0).index   = 0;
        entries.at(0).type    = GPUBindingResourceType::TEXTURE;
        entries.at(0).texture = texinfo.view;

        entries.at(1).index   = 1;
        entries.at(1).type    = GPUBindingResourceType::SAMPLER;
        entries.at(1).sampler = renderer_data->sampler;

        texinfo.bindgroup = execute([&]() {
            GPUBindGroupDescriptor desc{};
            desc.layout  = renderer_data->blayouts.at(1);
            desc.entries = entries;
            return device.create_bind_group(desc);
        });
    }
}

void GUIRenderer::setup_render_state(GPUCommandBuffer cmdbuffer, int width, int height)
{
    cmdbuffer.set_pipeline(renderer_data->pipeline);
    cmdbuffer.set_viewport(0, 0, (float)width, (float)height);
}

void GUIRenderer::create_window_context(WindowHandle window, ImGuiContext* context)
{
    auto& window_contexts = platform_data->window_contexts;
    window_contexts.emplace_back<GUIWindowContext>({window, context});
}

void GUIRenderer::delete_window_context(WindowHandle window)
{
    auto& window_contexts = platform_data->window_contexts;
    for (auto it = window_contexts.begin(); it != window_contexts.end(); it++)
        if (it->window.window == window.window)
            it = window_contexts.erase(it);
}
#pragma endregion GUIRenderer
