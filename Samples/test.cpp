#include <Common/Function.h>
#include <Render/Render.hpp>
#include <Window/Window.hpp>

using namespace lyra;
using namespace lyra::wsi;
using namespace lyra::rhi;

CString SHADER = R"""(
struct VertexInput
{
    float3 position : POSITION;
    float3 color    : COLOR0;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

[shader("vertex")]
VertexOutput vsmain(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.color = float4(input.color, 1.0);
    return output;
}

[shader("fragment")]
float4 fsmain(VertexOutput input) : SV_Target
{
    return input.color;
}
)""";

GPUShaderModule   vshader;
GPUShaderModule   fshader;
GPURenderPipeline pipeline;
GPUPipelineLayout playout;
GPUBuffer         vbuffer;
GPUBuffer         ibuffer;

void setup_pipeline()
{
    auto compiler = execute([&]() {
        auto desc   = CompilerDescriptor{};
        desc.target = CompileTarget::SPIRV;
        desc.flags  = CompileFlag::DEBUG | CompileFlag::REFLECT;
        return Compiler::init(desc);
    });

    auto module = compiler.compile(CompileDescriptor{
        .module = "test",
        .path   = "test.slang",
        .source = SHADER,
    });

    auto& device = RHI::get_current_device();

    vshader = execute([&]() {
        auto code  = module->get_shader_blob("vsmain");
        auto desc  = GPUShaderModuleDescriptor{};
        desc.label = "vertex shader";
        desc.data  = code->data;
        desc.size  = code->size;
        return device.create_shader_module(desc);
    });

    fshader = execute([&]() {
        auto code  = module->get_shader_blob("fsmain");
        auto desc  = GPUShaderModuleDescriptor{};
        desc.label = "fragment shader";
        desc.data  = code->data;
        desc.size  = code->size;
        return device.create_shader_module(desc);
    });

    playout = execute([&]() {
        GPUPipelineLayoutDescriptor desc = {};
        return device.create_pipeline_layout(desc);
    });

    pipeline = execute([&]() {
        auto position            = GPUVertexAttribute{};
        position.format          = GPUVertexFormat::FLOAT32x3;
        position.offset          = 0;
        position.shader_location = 0;

        auto color            = GPUVertexAttribute{};
        color.format          = GPUVertexFormat::FLOAT32x3;
        color.offset          = 0;
        color.shader_location = 1;

        auto layout         = GPUVertexBufferLayout{};
        layout.attributes   = {position, color};
        layout.array_stride = sizeof(float) * 6;
        layout.step_mode    = GPUVertexStepMode::VERTEX;

        auto target         = GPUColorTargetState{};
        target.format       = GPUTextureFormat::RGBA8UNORM;
        target.blend_enable = false;

        auto desc                                  = GPURenderPipelineDescriptor{};
        desc.layout                                = playout;
        desc.primitive.cull_mode                   = GPUCullMode::NONE;
        desc.primitive.topology                    = GPUPrimitiveTopology::TRIANGLE_LIST;
        desc.primitive.front_face                  = GPUFrontFace::CCW;
        desc.primitive.strip_index_format          = GPUIndexFormat::UINT32;
        desc.depth_stencil.depth_compare           = GPUCompareFunction::ALWAYS;
        desc.depth_stencil.depth_write_enabled     = false;
        desc.multisample.alpha_to_coverage_enabled = false;
        desc.multisample.count                     = 1;
        desc.vertex.module                         = vshader;
        desc.fragment.module                       = fshader;
        desc.vertex.buffers.push_back(layout);
        desc.fragment.targets.push_back(target);

        return device.create_render_pipeline(desc);
    });
}

void setup_buffers()
{
}

void cleanup()
{
    // NOTE: This is optional, because all resources will be automatically collected by device at destruction.
    vshader.destroy();
    fshader.destroy();
    playout.destroy();
    pipeline.destroy();
    // ibuffer.destroy();
    // vbuffer.destroy();
}

void update()
{
    // do some logic updates here
}

void render()
{
    auto& surface = RHI::get_current_surface();

    // acquire next frame from swapchain
    auto texture = surface.get_current_texture();

    // present this frame to swapchain
    texture.present();
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
        auto desc = GPUAdapterDescriptor{};
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
        auto desc   = GPUSurfaceDescriptor{};
        desc.label  = "main_surface";
        desc.window = win->handle;
        return rhi->request_surface(desc);
    });

    win->bind(WindowEvent::START, setup_pipeline);
    win->bind(WindowEvent::START, setup_buffers);
    win->bind(WindowEvent::CLOSE, cleanup);
    win->bind(WindowEvent::UPDATE, update);
    win->bind(WindowEvent::RENDER, render);
    win->bind(WindowEvent::RESIZE, resize);
    win->loop();

    return 0;
}
