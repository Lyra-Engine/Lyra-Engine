#include <Lyra/Lyra.hpp>

int main(int argc, const char* argv[])
{
    // common paths (TODO: need to think about what the project structure should look like)
    auto root       = lyra::git_root();
    auto asset_root = root; // / "Assets";
    auto cache_root = root; // / "Cache";

    // application
    auto app = lyra::execute([&]() {
        auto desc = lyra::AppDescriptor();
        desc.with_title("Lyra Engine :: Editor");
        desc.with_window_extent(1920, 1080);
        desc.with_graphics_backend(lyra::RHIBackend::VULKAN);
        desc.with_graphics_validation(true, true);
        return std::make_unique<lyra::Application>(desc);
    });

    // file loader
    auto file_loader = lyra::execute([&]() {
        auto loader = std::make_unique<lyra::FileLoader>(lyra::FSLoader::NATIVE);
        loader->mount("/", cache_root, 0);
        loader->mount("/", asset_root, 1);
        return loader;
    });

    // asset manager
    auto asset_manager = lyra::execute([&]() {
        auto desc                = lyra::AMSDescriptor{};
        desc.importer.asset_path = asset_root;
        desc.importer.cache_path = cache_root;
        desc.loader.assets       = file_loader->api();
        desc.loader.metadata     = file_loader->api();
        desc.watch               = true;
        desc.workers             = 4;
        return std::make_unique<lyra::AssetManager>(desc);
    });

    // imgui manager
    auto imgui_manager = lyra::execute([&]() {
        auto desc      = lyra::GUIDescriptor{};
        desc.window    = app->get_blackboard().get<lyra::WindowHandle>();
        desc.surface   = app->get_blackboard().get<lyra::GPUSurfaceHandle>();
        desc.compiler  = app->get_blackboard().get<lyra::CompilerHandle>();
        desc.docking   = true;
        desc.viewports = true;
        return std::make_unique<lyra::ImGuiManager>(desc);
    });

    // bind app bundles
    app->bind<lyra::ImGuiManager>(*imgui_manager);
    app->bind<lyra::AssetManager>(*asset_manager);

    // event loop
    app->run();
    return EXIT_SUCCESS;
}
