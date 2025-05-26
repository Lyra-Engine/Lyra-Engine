#include <Common/Function.h>
#include <Render/Render.hpp>
#include <Window/Window.hpp>

using namespace lyra;
using namespace lyra::rhi;

void setup()
{
}

void cleanup()
{
}

void update()
{
}

void render()
{
}

void resize()
{
}

int main()
{
    auto win = execute([&]() {
        auto desc   = WindowDescriptor{};
        desc.title  = "Lyra Engine :: Sample";
        desc.width  = 1920;
        desc.height = 1080;
        return Window(desc);
    });

    auto gpu = execute([&] {
        auto desc    = GPUDescriptor{};
        desc.backend = GPUBackend::VULKAN;
        desc.flags   = GPUFlag::DEBUG | GPUFlag::VALIDATION;
        desc.window  = win.handle;
        return GPU(desc);
    });

    auto adapter = execute([&]() {
        GPUAdapterDescriptor desc = {};
        return gpu.request_adapter(desc);
    });

    auto device = execute([&]() {
        auto desc  = GPUDeviceDescriptor{};
        desc.label = "main_device";
        desc.required_features.push_back(GPUFeatureName::SHADER_F16);
        desc.required_features.push_back(GPUFeatureName::FLOAT32_BLENDABLE);
        return adapter.request_device(desc);
    });

    auto surface = execute([&]() {
        auto desc  = GPUSurfaceDescriptor{};
        desc.label = "main_surface";
        return gpu.request_surface(device, desc);
    });

    win.bind(WindowEvent::START, setup);
    win.bind(WindowEvent::CLOSE, cleanup);
    win.bind(WindowEvent::UPDATE, update);
    win.bind(WindowEvent::RENDER, render);
    win.bind(WindowEvent::RESIZE, resize);
    win.loop();

    surface.destroy();
    device.destroy();
    gpu.destroy();
    win.destroy();
    return 0;
}
