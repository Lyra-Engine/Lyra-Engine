#include <Common/Function.h>
#include <Render/Render.hpp>
#include <Window/Window.hpp>

using namespace lyra;
using namespace lyra::wsi;
using namespace lyra::rhi;

GPUShaderModule   shader;
GPURenderPipeline pipeline;
GPUBuffer         vbuffer;
GPUBuffer         ibuffer;

void setup()
{
    // auto& device = GPU::get_current_device();
    //
    // shader = execute([&]() {
    //     auto desc  = GPUShaderModuleDescriptor{};
    //     desc.label = "vertex/fragment shader";
    //     desc.code  = {};
    //     return device.create_shader_module(desc);
    // });
    //
    // pipeline = execute([&]() {
    //     auto attrib            = GPUVertexAttribute{};
    //     attrib.format          = GPUVertexFormat::FLOAT32x3;
    //     attrib.offset          = 0;
    //     attrib.shader_location = 0;
    //
    //     auto layout         = GPUVertexBufferLayout{};
    //     layout.attributes   = {attrib};
    //     layout.array_stride = sizeof(float) * 3;
    //     layout.step_mode    = GPUVertexStepMode::VERTEX;
    //
    //     auto target   = GPUColorTargetState{};
    //     target.format = GPUTextureFormat::RGBA8UNORM;
    //     target.blend  = {};
    //
    //     auto desc                                  = GPURenderPipelineDescriptor{};
    //     desc.primitive.cull_mode                   = GPUCullMode::NONE;
    //     desc.primitive.topology                    = GPUPrimitiveTopology::TRIANGLE_LIST;
    //     desc.primitive.front_face                  = GPUFrontFace::CCW;
    //     desc.primitive.strip_index_format          = GPUIndexFormat::UINT32;
    //     desc.depth_stencil.depth_write_enabled     = false;
    //     desc.multisample.alpha_to_coverage_enabled = false;
    //     desc.multisample.count                     = 1;
    //     desc.vertex.module                         = shader;
    //     desc.vertex.entry_point                    = "vertex";
    //     desc.fragment.module                       = shader;
    //     desc.fragment.entry_point                  = "fragment";
    //     desc.vertex.buffers.push_back(layout);
    //     desc.fragment.targets.push_back(target);
    //     return device.create_render_pipeline(desc);
    // });
}

void cleanup()
{
    // shader.destroy();
    // pipeline.destroy();
    // ibuffer.destroy();
    // vbuffer.destroy();
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
        return Window::init(desc);
    });

    auto rhi = execute([&] {
        auto desc    = RHIDescriptor{};
        desc.backend = RHIBackend::VULKAN;
        desc.flags   = RHIFlag::DEBUG | RHIFlag::VALIDATION;
        desc.window  = win->handle;
        return RHI::init(desc);
    });

    auto adapter = execute([&]() {
        GPUAdapterDescriptor desc = {};
        return rhi->request_adapter(desc);
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
        return rhi->request_surface(desc);
    });

    win->bind(WindowEvent::START, setup);
    win->bind(WindowEvent::CLOSE, cleanup);
    win->bind(WindowEvent::UPDATE, update);
    win->bind(WindowEvent::RENDER, render);
    win->bind(WindowEvent::RESIZE, resize);
    win->loop();

    return 0;
}
