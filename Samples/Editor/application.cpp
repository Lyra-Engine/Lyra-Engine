#include "application.h"
#include "Lyra/Common/Function.h"
#include "imgui.h"

void Application::init()
{
    // initialize application logger
    logger = init_stderr_logger("Editor", LogLevel::trace);

    // initialize GPU RHI
    rhi = lyra::execute([&]() {
        auto desc    = RHIDescriptor{};
        desc.backend = this->desc.rhi_backend;
        desc.flags   = this->desc.rhi_flags;
        desc.window  = this->desc.window;
        return RHI::init(desc);
    });

    // initialize shader compiler
    slc = lyra::execute([&]() {
        auto desc   = CompilerDescriptor{};
        desc.target = this->desc.slc_target;
        desc.flags  = this->desc.slc_flags;
        return Compiler::init(desc);
    });

    // initialize GPU adatper
    adapter = lyra::execute([&]() {
        auto desc = GPUAdapterDescriptor{};
        return rhi->request_adapter(desc);
    });

    // initialize GPU device
    device = lyra::execute([&]() {
        auto desc  = GPUDeviceDescriptor{};
        desc.label = "main_device";
        return adapter.request_device(desc);
    });

    // initialize GPU surface/swapchain
    surface = lyra::execute([&]() {
        auto desc         = GPUSurfaceDescriptor{};
        desc.label        = "main_surface";
        desc.window       = this->desc.window;
        desc.present_mode = GPUPresentMode::Fifo;
        return rhi->request_surface(desc);
    });

    // initialize GUI renderer
    gui = lyra::execute([&]() {
        auto desc      = GUIDescriptor{};
        desc.window    = this->desc.window;
        desc.surface   = surface;
        desc.compiler  = *slc;
        desc.viewports = true;
        return GUI::init(desc);
    });
}

void Application::destroy()
{
    rhi->wait();
}

void Application::update(const Window& window)
{
    gui->update();
    gui->ui([&]() {
        ImGui::ShowDemoWindow();
    });
}

void Application::render()
{
    // command buffer
    auto command = execute([&]() {
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
    gui->render_main_viewport(command, backbuffer.view);
    command.resource_barrier(state_transition(backbuffer.texture, color_attachment_state(), present_src_state()));

    // command buffer submission
    command.submit();

    // swapchain presentation
    backbuffer.present();

    // render other platform window viewports
    gui->render_side_viewports();
}

void Application::resize()
{
}
