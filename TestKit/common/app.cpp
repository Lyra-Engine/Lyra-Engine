#include "./app.h"

TestApp::TestApp(const TestAppDescriptor& app_desc) : desc(app_desc)
{
    // initialize window
    if (desc.window) {
        auto desc   = WindowDescriptor{};
        desc.title  = "Lyra Engine :: Test";
        desc.width  = app_desc.width;
        desc.height = app_desc.height;
        win         = Window::init(desc);
    }

    // initialize rhi
    if (true) {
        auto desc    = RHIDescriptor{};
        desc.backend = app_desc.backend;
        desc.flags   = app_desc.rhi_flags;
        if (app_desc.window)
            desc.window = win->handle;
        rhi = RHI::init(desc);
    }

    // initialize GPU adatper
    auto adapter = execute([&]() {
        auto desc = GPUAdapterDescriptor{};
        return rhi->request_adapter(desc);
    });

    // initialize GPU device
    auto device = execute([&]() {
        auto desc  = GPUDeviceDescriptor{};
        desc.label = "main_device";
        desc.required_features.push_back(GPUFeatureName::SHADER_F16);
        desc.required_features.push_back(GPUFeatureName::FLOAT32_BLENDABLE);
        return adapter.request_device(desc);
    });

    // initialize swapchain
    if (desc.window) {
        auto desc         = GPUSurfaceDescriptor{};
        desc.label        = "main_surface";
        desc.window       = win->handle;
        desc.present_mode = GPUPresentMode::Fifo;
        rhi->request_surface(desc);
    }

    // initialize compiler
    compiler = execute([&]() {
        auto desc   = CompilerDescriptor{};
        desc.target = app_desc.compile_target;
        desc.flags  = app_desc.compile_flags;
        return Compiler::init(desc);
    });
}

void TestApp::run()
{
    render_target = RenderTarget::create(get_backbuffer_format(), desc.width, desc.height);

    if (desc.window) {
        run_with_window();
    } else {
        run_without_window();
    }
}

void TestApp::run_with_window()
{
    win->bind<WindowEvent::RENDER>([&]() {
        auto& surface = RHI::get_current_surface();
        auto  texture = surface.get_current_texture();
        if (!texture.suboptimal) {
            render(texture);
            texture.present();
        }
    });
    win->bind<WindowEvent::CLOSE>([&]() {
        RHI::get_current_device().wait();
    });
    win->loop();
}

void TestApp::run_without_window()
{
    GPUSurfaceTexture backbuffer;
    backbuffer.texture = render_target.texture.handle;
    backbuffer.view    = render_target.view.handle;
    render(backbuffer);

    auto& device = RHI::get_current_device();
    device.wait();

    String name = String(desc.name) + ".png";
    render_target.save(name.c_str());
}

void TestApp::postprocessing(const GPUCommandBuffer& cmd, GPUTextureHandle backbuffer)
{
    if (desc.window) {
        cmd.resource_barrier(state_transition(backbuffer, color_attachment_state(), present_src_state()));
    } else {
        cmd.resource_barrier(state_transition(backbuffer, color_attachment_state(), copy_src_state()));
        cmd.copy_texture_to_buffer(render_target.copy_src(), render_target.copy_dst(), render_target.copy_ext());
    }
}

GPUTextureFormat TestApp::get_backbuffer_format() const
{
    if (desc.window) {
        return RHI::get_current_surface().get_current_format();
    } else {
        return GPUTextureFormat::RGBA8UNORM;
    }
}
