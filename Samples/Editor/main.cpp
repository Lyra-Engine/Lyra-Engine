#include <Lyra/Lyra.hpp>

using namespace lyra;

void imgui_update(Blackboard& blackboard)
{
    // create a fullscreen window for docking
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // create the docking space
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();

    ImGui::ShowDemoWindow();
    ImGui::ShowDebugLogWindow();
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
        return std::make_unique<ImGuiManager>(desc);
    });

    // bind app bundles
    app->bind<ImGuiManager>(*imgui_manager);
    app->bind<AssetManager>(*asset_manager);

    // bind additional systems
    app->bind<AppEvent::UPDATE>(imgui_update);
    app->bind<AppEvent::RENDER>(imgui_render);

    // event loop
    app->run();
    return EXIT_SUCCESS;
}
