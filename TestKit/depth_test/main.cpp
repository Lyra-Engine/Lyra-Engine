#include "helper.h"

CString depth_test_program = R"""(
struct VertexInput
{
    float3 position : ATTRIBUTE0;
    float3 color    : ATTRIBUTE1;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

struct Camera
{
    float4x4 proj;
    float4x4 view;
};

ConstantBuffer<Camera> camera;

[shader("vertex")]
VertexOutput vsmain(VertexInput input)
{
    VertexOutput output;
    output.position = mul(mul(float4(input.position, 1.0), camera.view), camera.proj);
    output.color = float4(input.color, 1.0);
    return output;
}

[shader("fragment")]
float4 fsmain(VertexOutput input) : SV_Target
{
    return input.color;
}
)""";

struct DepthTestApp : public TestApp
{
    Uniform            uniform;
    Geometry           geometry;
    GPUShaderModule    vshader;
    GPUShaderModule    fshader;
    GPURenderPipeline  pipeline;
    GPUPipelineLayout  playout;
    GPUBindGroupLayout blayout;
    GPUTexture         dbuffer;
    GPUTextureView     dview;

    explicit DepthTestApp(const TestAppDescriptor& desc) : TestApp(desc)
    {
        setup_depth();
        setup_buffers();
        setup_pipeline();
    }

    void setup_depth()
    {
        auto& device = RHI::get_current_device();

        dbuffer = execute([&]() {
            auto desc            = GPUTextureDescriptor{};
            desc.format          = GPUTextureFormat::DEPTH16UNORM;
            desc.size.width      = this->desc.width;
            desc.size.height     = this->desc.height;
            desc.size.depth      = 1;
            desc.array_layers    = 1;
            desc.mip_level_count = 1;
            desc.usage           = GPUTextureUsage::RENDER_ATTACHMENT;
            desc.label           = "depth_buffer";
            return device.create_texture(desc);
        });

        dview = dbuffer.create_view();
    }

    void setup_buffers()
    {
        uint  width  = desc.width;
        uint  height = desc.height;
        float fovy   = 1.05;
        float aspect = float(width) / float(height);

        geometry = Geometry::create_overlapping_triangles();
        uniform  = Uniform::create(fovy, aspect, glm::vec3(0.0, 0.0, 3.0));
    }

    void setup_pipeline()
    {
        auto& device = RHI::get_current_device();

        auto module = execute([&]() {
            auto desc   = CompileDescriptor{};
            desc.module = "test";
            desc.path   = "test.slang";
            desc.source = depth_test_program;
            return compiler->compile(desc);
        });

        vshader = execute([&]() {
            auto code  = module->get_shader_blob("vsmain");
            auto desc  = GPUShaderModuleDescriptor{};
            desc.label = "vertex_shader";
            desc.data  = code->data;
            desc.size  = code->size;
            return device.create_shader_module(desc);
        });

        fshader = execute([&]() {
            auto code  = module->get_shader_blob("fsmain");
            auto desc  = GPUShaderModuleDescriptor{};
            desc.label = "fragment_shader";
            desc.data  = code->data;
            desc.size  = code->size;
            return device.create_shader_module(desc);
        });

        blayout = execute([&]() {
            auto entry                      = GPUBindGroupLayoutEntry{};
            entry.type                      = GPUBindingResourceType::BUFFER;
            entry.binding                   = 0;
            entry.count                     = 1;
            entry.visibility                = GPUShaderStage::VERTEX;
            entry.buffer.type               = GPUBufferBindingType::UNIFORM;
            entry.buffer.has_dynamic_offset = false;

            auto desc    = GPUBindGroupLayoutDescriptor{};
            desc.entries = entry;
            return device.create_bind_group_layout(desc);
        });

        playout = execute([&]() {
            auto desc               = GPUPipelineLayoutDescriptor{};
            desc.bind_group_layouts = blayout.handle;
            return device.create_pipeline_layout(desc);
        });

        pipeline = execute([&]() {
            Array<GPUVertexAttribute, 2> attributes;

            auto& position           = attributes.at(0);
            position.format          = GPUVertexFormat::FLOAT32x3;
            position.offset          = offsetof(Vertex, position);
            position.shader_location = 0;

            auto& color           = attributes.at(1);
            color.format          = GPUVertexFormat::FLOAT32x3;
            color.offset          = offsetof(Vertex, color);
            color.shader_location = 1;

            auto layout         = GPUVertexBufferLayout{};
            layout.attributes   = attributes;
            layout.array_stride = sizeof(Vertex);
            layout.step_mode    = GPUVertexStepMode::VERTEX;

            auto target         = GPUColorTargetState{};
            target.format       = get_backbuffer_format();
            target.blend_enable = false;

            auto desc                                  = GPURenderPipelineDescriptor{};
            desc.layout                                = playout;
            desc.primitive.cull_mode                   = GPUCullMode::NONE;
            desc.primitive.topology                    = GPUPrimitiveTopology::TRIANGLE_LIST;
            desc.primitive.front_face                  = GPUFrontFace::CCW;
            desc.primitive.strip_index_format          = GPUIndexFormat::UINT32;
            desc.depth_stencil.format                  = GPUTextureFormat::DEPTH16UNORM;
            desc.depth_stencil.depth_compare           = GPUCompareFunction::LESS;
            desc.depth_stencil.depth_write_enabled     = true;
            desc.multisample.alpha_to_coverage_enabled = false;
            desc.multisample.count                     = 1;
            desc.vertex.module                         = vshader;
            desc.fragment.module                       = fshader;
            desc.vertex.buffers                        = layout;
            desc.fragment.targets                      = target;

            return device.create_render_pipeline(desc);
        });
    }

    void render(const GPUSurfaceTexture& backbuffer) override
    {
        auto& device = RHI::get_current_device();

        // create command buffer
        auto command = execute([&]() {
            auto desc  = GPUCommandBufferDescriptor{};
            desc.queue = GPUQueueType::DEFAULT;
            return device.create_command_buffer(desc);
        });

        // create bind group
        auto bind_group = execute([&]() {
            Array<GPUBindGroupEntry, 1> entries = {};

            auto& entry         = entries.at(0);
            entry.type          = GPUBindingResourceType::BUFFER;
            entry.binding       = 0;
            entry.buffer.buffer = uniform.ubuffer;
            entry.buffer.offset = 0;
            entry.buffer.size   = 0;

            auto desc    = GPUBindGroupDescriptor{};
            desc.layout  = blayout;
            desc.entries = entries;
            return device.create_bind_group(desc);
        });

        // color attachments
        auto color_attachment        = GPURenderPassColorAttachment{};
        color_attachment.clear_value = GPUColor{0.0f, 0.0f, 0.0f, 0.0f};
        color_attachment.load_op     = GPULoadOp::CLEAR;
        color_attachment.store_op    = GPUStoreOp::STORE;
        color_attachment.view        = backbuffer.view;

        auto depth_attachment              = GPURenderPassDepthStencilAttachment{};
        depth_attachment.view              = dview;
        depth_attachment.depth_clear_value = 1.0f;
        depth_attachment.depth_load_op     = GPULoadOp::CLEAR;
        depth_attachment.depth_store_op    = GPUStoreOp::STORE;
        depth_attachment.depth_read_only   = false;

        // render pass info
        auto render_pass                     = GPURenderPassDescriptor{};
        render_pass.color_attachments        = color_attachment;
        render_pass.depth_stencil_attachment = depth_attachment;

        // synchronization when window is enabled
        if (desc.window) {
            command.wait(backbuffer.available, GPUBarrierSync::PIXEL_SHADING);
            command.signal(backbuffer.complete, GPUBarrierSync::RENDER_TARGET);
        }

        // commands
        command.resource_barrier(state_transition(backbuffer.texture, undefined_state(), color_attachment_state()));
        command.resource_barrier(state_transition(dbuffer, undefined_state(), depth_stencil_attachment_state()));
        command.begin_render_pass(render_pass);
        command.set_viewport(0, 0, desc.width, desc.height);
        command.set_scissor_rect(0, 0, desc.width, desc.height);
        command.set_pipeline(pipeline);
        command.set_vertex_buffer(0, geometry.vbuffer);
        command.set_index_buffer(geometry.ibuffer, GPUIndexFormat::UINT32);
        command.set_bind_group(0, bind_group);
        command.draw_indexed(6, 1, 0, 0, 0);
        command.end_render_pass();
        postprocessing(command, backbuffer.texture);
        command.submit();
    }
};

TEST_CASE("rhi::vulkan::depth_test" * doctest::description("Rendering a triangle with depth test enabled."))
{
    TestAppDescriptor desc{};
    desc.name           = "vulkan";
    desc.window         = false;
    desc.backend        = RHIBackend::VULKAN;
    desc.width          = 640;
    desc.height         = 480;
    desc.rhi_flags      = RHIFlag::DEBUG | RHIFlag::VALIDATION;
    desc.compile_target = CompileTarget::SPIRV;
    desc.compile_flags  = CompileFlag::DEBUG;
    DepthTestApp(desc).run();
}

#ifdef WIN32
TEST_CASE("rhi::d3d12::depth_test" * doctest::description("Rendering a triangle with depth test enabled."))
{
    TestAppDescriptor desc{};
    desc.name           = "d3d12";
    desc.window         = false;
    desc.backend        = RHIBackend::D3D12;
    desc.width          = 640;
    desc.height         = 480;
    desc.rhi_flags      = RHIFlag::DEBUG | RHIFlag::VALIDATION;
    desc.compile_target = CompileTarget::DXIL;
    desc.compile_flags  = CompileFlag::DEBUG;
    DepthTestApp(desc).run();
}
#endif
