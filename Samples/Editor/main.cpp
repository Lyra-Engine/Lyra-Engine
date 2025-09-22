#include <cxxopts.hpp>
#include <Lyra/Lyra.hpp>

using namespace lyra;

void imgui_update(Blackboard& blackboard)
{
    auto& layout = blackboard.get<LayoutInfo>();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create")) {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
            }
            if (ImGui::MenuItem("Save as..")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::DockBuilderDockWindow("Dear ImGui Demo", layout.main);
    ImGui::ShowDemoWindow();

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
    auto root           = git_root();
    auto assets_root    = root; // / "Assets";
    auto metadata_root  = root; // / "Metadata";
    auto generated_root = root; // / "Generated";

    // file loader
    auto file_loader = lyra::execute([&]() {
        auto loader = std::make_unique<FileLoader>(FSLoader::NATIVE);
        loader->mount("/", generated_root, 2);
        loader->mount("/", metadata_root, 1);
        loader->mount("/", assets_root, 0);
        return loader;
    });

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

    // console manager
    auto theme_manager   = std::make_unique<ThemeManager>();
    auto console_manager = std::make_unique<ConsoleManager>(4096);

    // asset manager
    auto asset_manager = lyra::execute([&]() {
        auto desc                    = AMSDescriptor{};
        desc.importer.assets_path    = assets_root.c_str();
        desc.importer.metadata_path  = metadata_root.c_str();
        desc.importer.generated_path = generated_root.c_str();
        desc.loader.assets           = file_loader.get();
        desc.loader.metadata         = file_loader.get();
        desc.watch                   = true;
        desc.workers                 = 4;
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

        auto imgui = std::make_unique<GUIManager>(desc);
        imgui->apply_context(); // imgui context in user application
        return std::move(imgui);
    });

    // layout manager
    auto layout_manager = lyra::execute([&]() {
        auto desc   = LayoutDescriptor{};
        desc.mode   = LayoutMode::EDITOR;
        desc.left   = 0.2f;
        desc.right  = 0.3f;
        desc.top    = 0.2f;
        desc.bottom = 0.3f;
        return std::make_unique<LayoutManager>(desc);
    });

    // bind app bundles
    app->bind<GUIManager>(*imgui_manager);
    app->bind<AssetManager>(*asset_manager);
    app->bind<ThemeManager>(*theme_manager);
    app->bind<LayoutManager>(*layout_manager);
    app->bind<ConsoleManager>(*console_manager);

    // bind additional systems
    app->bind<AppEvent::UPDATE>(imgui_update);
    app->bind<AppEvent::RENDER>(imgui_render);

    // event loop
    app->run();
    return EXIT_SUCCESS;
}
