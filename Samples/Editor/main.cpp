#include <Lyra/Lyra.hpp>

using namespace lyra;

void imgui_update(Blackboard& blackboard)
{
    auto& layout = blackboard.get<LayoutInfo>();

    ImGuiIO& io  = ImGui::GetIO();
    float    fps = io.Framerate;

    ImGui::DockBuilderDockWindow("Dear ImGui Demo", layout.main);
    ImGui::ShowDemoWindow();

    ImGui::DockBuilderDockWindow("Dear ImGui Debug Log", layout.bottom);
    ImGui::ShowDebugLogWindow();

    ImGui::DockBuilderDockWindow("Scene", layout.main);
    ImGui::Begin("Scene");
    ImGui::End();

    ImGui::DockBuilderDockWindow("Hierarchy", layout.left);
    ImGui::Begin("Hierarchy");
    ImGui::Text("Hi, World!");
    ImGui::End();

    ImGui::DockBuilderDockWindow("Inspector", layout.right);
    ImGui::Begin("Inspector");
    ImGui::End();

    ImGui::DockBuilderDockWindow("Console", layout.bottom);
    ImGui::Begin("Console");
    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("\uea84 \uf09b \ue65b Hello World \ue8a9 \ue200 \ue201 \ue202 \ue404");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Text("FPS: %.3f", fps);
    ImGui::End();
}

void imgui_render(Blackboard& blackboard)
{
    auto device  = blackboard.get<GPUDevice>();
    auto surface = blackboard.get<GPUSurface>();
    auto imgui   = blackboard.get<GUIRenderer*>();

    // command buffer
    auto command = lyra::execute([&]() {
        auto desc  = GPUCommandBufferDescriptor{};
        desc.queue = GPUQueueType::DEFAULT;
        return device.create_command_buffer(desc);
    });

    // current backbuffer
    auto backbuffer = surface.get_current_texture();

    // synchronization
    command.wait(backbuffer.available, GPUBarrierSync::PIXEL_SHADING);
    command.signal(backbuffer.complete, GPUBarrierSync::RENDER_TARGET);

    // command recording
    command.resource_barrier(state_transition(backbuffer.texture, undefined_state(), color_attachment_state()));
    imgui->render_main_viewport(command, backbuffer.view);
    command.resource_barrier(state_transition(backbuffer.texture, color_attachment_state(), present_src_state()));

    // command buffer submission
    command.submit();

    // swapchain presentation
    backbuffer.present();
}

int main(int argc, const char* argv[])
{
    // common paths (TODO: need to think about what the project structure should look like)
    auto root       = git_root();
    auto asset_root = root; // / "Assets";
    auto cache_root = root; // / "Cache";

    // application
    auto app = lyra::execute([&]() {
        auto desc = AppDescriptor();
        desc.with_title("Lyra Engine :: Editor");
        desc.with_window_extent(1920, 1080);
        desc.with_window_maximized();
        desc.with_graphics_backend(RHIBackend::VULKAN);
        desc.with_graphics_validation(true, true);
        return std::make_unique<Application>(desc);
    });

    // file loader
    auto file_loader = lyra::execute([&]() {
        auto loader = std::make_unique<FileLoader>(FSLoader::NATIVE);
        loader->mount("/", cache_root, 0);
        loader->mount("/", asset_root, 1);
        return loader;
    });

    // asset manager
    auto asset_manager = lyra::execute([&]() {
        auto desc                = AMSDescriptor{};
        desc.importer.asset_path = asset_root;
        desc.importer.cache_path = cache_root;
        desc.loader.assets       = file_loader->api();
        desc.loader.metadata     = file_loader->api();
        desc.watch               = true;
        desc.workers             = 4;
        return std::make_unique<AssetManager>(desc);
    });

    // imgui manager
    auto imgui_manager = lyra::execute([&]() {
        auto desc      = GUIDescriptor{};
        desc.window    = app->get_blackboard().get<Window>();
        desc.surface   = app->get_blackboard().get<GPUSurface>();
        desc.compiler  = app->get_blackboard().get<Compiler>();
        desc.docking   = true;
        desc.viewports = true;
        return std::make_unique<GUIManager>(desc);
    });

    // layout manager
    auto layout_manager = lyra::execute([&]() {
        auto desc   = LayoutDescriptor{};
        desc.mode   = LayoutMode::EDITOR;
        desc.left   = 0.2f;
        desc.right  = 0.3f;
        desc.top    = 0.2f;
        desc.bottom = 0.2f;
        return std::make_unique<LayoutManager>(desc);
    });

    // imgui context in user application
    imgui_manager->apply_context();

    // bind app bundles
    app->bind<GUIManager>(*imgui_manager);
    app->bind<AssetManager>(*asset_manager);
    app->bind<LayoutManager>(*layout_manager);

    // bind additional systems
    app->bind<AppEvent::UPDATE>(imgui_update);
    app->bind<AppEvent::RENDER>(imgui_render);

    // event loop
    app->run();
    return EXIT_SUCCESS;
}
