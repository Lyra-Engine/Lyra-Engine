#include "application.h"

int main(int argc, const char* argv[])
{
    auto win = lyra::execute([&]() {
        auto desc   = lyra::wsi::WindowDescriptor{};
        desc.title  = "Lyra Engine :: Editor";
        desc.width  = 1920;
        desc.height = 1080;
        return Window::init(desc);
    });

#if 1
    auto app = lyra::execute([&]() {
        auto desc        = AppDescriptor{};
        desc.window      = win->handle;
        desc.rhi_backend = RHIBackend::VULKAN;
        desc.rhi_flags   = RHIFlag::DEBUG | RHIFlag::VALIDATION;
        desc.slc_target  = CompileTarget::SPIRV;
        desc.slc_flags   = CompileFlag::DEBUG | CompileFlag::REFLECT;
        return Application(desc);
    });
#else
    auto app = lyra::execute([&]() {
        auto desc        = AppDescriptor{};
        desc.window      = win->handle;
        desc.rhi_backend = RHIBackend::D3D12;
        desc.rhi_flags   = RHIFlag::DEBUG | RHIFlag::VALIDATION;
        desc.slc_target  = CompileTarget::DXIL;
        desc.slc_flags   = CompileFlag::DEBUG | CompileFlag::REFLECT;
        return Application(desc);
    });
#endif

    // bind window callbacks
    win->bind<WindowEvent::START>(&Application::init, &app);
    win->bind<WindowEvent::UPDATE>(&Application::update, &app);
    win->bind<WindowEvent::RENDER>(&Application::render, &app);
    win->bind<WindowEvent::RESIZE>(&Application::resize, &app);
    win->bind<WindowEvent::CLOSE>(&Application::destroy, &app);

    // run event loop
    EventLoop::bind(*win);
    EventLoop::run();
    return EXIT_SUCCESS;
}
