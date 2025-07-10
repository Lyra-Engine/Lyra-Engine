#include "helper.h"

CString stencil_test_program = R"""(
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

struct StencilTestApp : public TestApp
{
    Uniform            uniform;
    Geometry           mask;
    Geometry           geometry;
    GPUShaderModule    vshader;
    GPUShaderModule    fshader;
    GPURenderPipeline  pipeline_mask;
    GPURenderPipeline  pipeline_draw;
    GPUPipelineLayout  playout;
    GPUBindGroupLayout blayout;
    GPUTexture         dsbuffer;
    GPUTextureView     dsview;

    explicit StencilTestApp(const TestAppDescriptor& desc) : TestApp(desc)
    {
        setup_stencil();
        setup_buffers();
        setup_pipeline();
    }

    void setup_stencil()
    {
        auto& device = RHI::get_current_device();

        dsbuffer = execute([&]() {
            auto desc            = GPUTextureDescriptor{};
            desc.format          = GPUTextureFormat::DEPTH24PLUS_STENCIL8;
            desc.size.width      = this->desc.width;
            desc.size.height     = this->desc.height;
            desc.size.depth      = 1;
            desc.array_layers    = 1;
            desc.mip_level_count = 1;
            desc.usage           = GPUTextureUsage::RENDER_ATTACHMENT;
            desc.label           = "depth_stencil_buffer";
            return device.create_texture(desc);
        });

        dsview = dsbuffer.create_view();
    }

    void setup_buffers()
    {
        uint  width  = desc.width;
        uint  height = desc.height;
        float fovy   = 1.05;
        float aspect = float(width) / float(height);

        mask     = Geometry::create_triangle();
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
            desc.source = stencil_test_program;
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
            auto desc                       = GPUBindGroupLayoutDescriptor{};
            auto entry                      = GPUBindGroupLayoutEntry{};
            entry.type                      = GPUBindingResourceType::BUFFER;
            entry.binding                   = 0;
            entry.count                     = 1;
            entry.visibility                = GPUShaderStage::VERTEX;
            entry.buffer.type               = GPUBufferBindingType::UNIFORM;
            entry.buffer.has_dynamic_offset = false;
            desc.entries.push_back(entry);
            return device.create_bind_group_layout(desc);
        });

        playout = execute([&]() {
            auto desc               = GPUPipelineLayoutDescriptor{};
            desc.bind_group_layouts = {blayout};
            return device.create_pipeline_layout(desc);
        });

        pipeline_mask = execute([&]() {
            auto position            = GPUVertexAttribute{};
            position.format          = GPUVertexFormat::FLOAT32x3;
            position.offset          = offsetof(Vertex, position);
            position.shader_location = 0;

            auto color            = GPUVertexAttribute{};
            color.format          = GPUVertexFormat::FLOAT32x3;
            color.offset          = offsetof(Vertex, color);
            color.shader_location = 1;

            auto layout         = GPUVertexBufferLayout{};
            layout.attributes   = {position, color};
            layout.array_stride = sizeof(Vertex);
            layout.step_mode    = GPUVertexStepMode::VERTEX;

            auto target         = GPUColorTargetState{};
            target.format       = get_backbuffer_format();
            target.blend_enable = false;
            target.write_mask   = GPUColorWrite::NONE; // NOTE: disable color write

            auto desc                                      = GPURenderPipelineDescriptor{};
            desc.layout                                    = playout;
            desc.primitive.cull_mode                       = GPUCullMode::NONE;
            desc.primitive.topology                        = GPUPrimitiveTopology::TRIANGLE_LIST;
            desc.primitive.front_face                      = GPUFrontFace::CCW;
            desc.primitive.strip_index_format              = GPUIndexFormat::UINT32;
            desc.depth_stencil.format                      = GPUTextureFormat::DEPTH24PLUS_STENCIL8;
            desc.depth_stencil.depth_compare               = GPUCompareFunction::ALWAYS;
            desc.depth_stencil.depth_write_enabled         = false;
            desc.depth_stencil.stencil_read_mask           = 0x0;
            desc.depth_stencil.stencil_write_mask          = 0x1;
            desc.depth_stencil.stencil_front.depth_fail_op = GPUStencilOperation::KEEP;
            desc.depth_stencil.stencil_front.compare       = GPUCompareFunction::ALWAYS;
            desc.depth_stencil.stencil_front.pass_op       = GPUStencilOperation::REPLACE;
            desc.depth_stencil.stencil_front.fail_op       = GPUStencilOperation::REPLACE;
            desc.depth_stencil.stencil_back                = desc.depth_stencil.stencil_front;
            desc.multisample.alpha_to_coverage_enabled     = false;
            desc.multisample.count                         = 1;
            desc.vertex.module                             = vshader;
            desc.fragment.module                           = fshader;
            desc.vertex.buffers.push_back(layout);
            desc.fragment.targets.push_back(target);

            return device.create_render_pipeline(desc);
        });

        pipeline_draw = execute([&]() {
            auto position            = GPUVertexAttribute{};
            position.format          = GPUVertexFormat::FLOAT32x3;
            position.offset          = offsetof(Vertex, position);
            position.shader_location = 0;

            auto color            = GPUVertexAttribute{};
            color.format          = GPUVertexFormat::FLOAT32x3;
            color.offset          = offsetof(Vertex, color);
            color.shader_location = 1;

            auto layout         = GPUVertexBufferLayout{};
            layout.attributes   = {position, color};
            layout.array_stride = sizeof(Vertex);
            layout.step_mode    = GPUVertexStepMode::VERTEX;

            auto target         = GPUColorTargetState{};
            target.format       = get_backbuffer_format();
            target.blend_enable = false;

            auto desc                                      = GPURenderPipelineDescriptor{};
            desc.layout                                    = playout;
            desc.primitive.cull_mode                       = GPUCullMode::NONE;
            desc.primitive.topology                        = GPUPrimitiveTopology::TRIANGLE_LIST;
            desc.primitive.front_face                      = GPUFrontFace::CCW;
            desc.primitive.strip_index_format              = GPUIndexFormat::UINT32;
            desc.depth_stencil.format                      = GPUTextureFormat::DEPTH24PLUS_STENCIL8;
            desc.depth_stencil.depth_compare               = GPUCompareFunction::ALWAYS;
            desc.depth_stencil.depth_write_enabled         = true;
            desc.depth_stencil.stencil_read_mask           = 0x1;
            desc.depth_stencil.stencil_write_mask          = 0x0;
            desc.depth_stencil.stencil_front.depth_fail_op = GPUStencilOperation::KEEP;
            desc.depth_stencil.stencil_front.compare       = GPUCompareFunction::EQUAL;
            desc.depth_stencil.stencil_front.pass_op       = GPUStencilOperation::KEEP; // don't modify stencil
            desc.depth_stencil.stencil_front.fail_op       = GPUStencilOperation::KEEP; // don't modify stencil
            desc.depth_stencil.stencil_back                = desc.depth_stencil.stencil_front;
            desc.multisample.alpha_to_coverage_enabled     = false;
            desc.multisample.count                         = 1;
            desc.vertex.module                             = vshader;
            desc.fragment.module                           = fshader;
            desc.vertex.buffers.push_back(layout);
            desc.fragment.targets.push_back(target);

            return device.create_render_pipeline(desc);
        });
    }

    void render_mask(GPUCommandBuffer& command, const GPUSurfaceTexture& backbuffer, const GPUBindGroup& bind_group)
    {
        // color attachments
        auto color_attachment        = GPURenderPassColorAttachment{};
        color_attachment.clear_value = GPUColor{0.0f, 0.0f, 0.0f, 0.0f};
        color_attachment.load_op     = GPULoadOp::CLEAR;
        color_attachment.store_op    = GPUStoreOp::DISCARD;
        color_attachment.view        = backbuffer.view;

        // stencil attachment
        auto stencil_attachment                = GPURenderPassDepthStencilAttachment{};
        stencil_attachment.view                = dsview;
        stencil_attachment.depth_clear_value   = 1.0f;
        stencil_attachment.depth_load_op       = GPULoadOp::CLEAR;
        stencil_attachment.depth_store_op      = GPUStoreOp::DISCARD;
        stencil_attachment.depth_read_only     = true;
        stencil_attachment.stencil_clear_value = 0;
        stencil_attachment.stencil_load_op     = GPULoadOp::CLEAR;
        stencil_attachment.stencil_store_op    = GPUStoreOp::STORE;
        stencil_attachment.stencil_read_only   = false;

        // render pass info
        auto render_pass                     = GPURenderPassDescriptor{};
        render_pass.color_attachments        = {color_attachment};
        render_pass.depth_stencil_attachment = stencil_attachment;

        command.begin_render_pass(render_pass);
        command.set_viewport(0, 0, desc.width, desc.height);
        command.set_scissor_rect(0, 0, desc.width, desc.height);
        command.set_pipeline(pipeline_mask);
        command.set_vertex_buffer(0, mask.vbuffer);
        command.set_index_buffer(mask.ibuffer, GPUIndexFormat::UINT32);
        command.set_bind_group(0, bind_group);
        command.set_stencil_reference(0x1);
        command.draw_indexed(3, 1, 0, 0, 0);
        command.end_render_pass();
    }

    void render_color(GPUCommandBuffer& command, const GPUSurfaceTexture& backbuffer, const GPUBindGroup& bind_group)
    {
        // color attachments
        auto color_attachment        = GPURenderPassColorAttachment{};
        color_attachment.clear_value = GPUColor{0.0f, 0.0f, 0.0f, 0.0f};
        color_attachment.load_op     = GPULoadOp::CLEAR;
        color_attachment.store_op    = GPUStoreOp::STORE;
        color_attachment.view        = backbuffer.view;

        // depth attachment
        auto depth_attachment                = GPURenderPassDepthStencilAttachment{};
        depth_attachment.view                = dsview;
        depth_attachment.depth_clear_value   = 1.0f;
        depth_attachment.depth_load_op       = GPULoadOp::CLEAR;
        depth_attachment.depth_store_op      = GPUStoreOp::STORE;
        depth_attachment.depth_read_only     = false;
        depth_attachment.stencil_clear_value = 0;
        depth_attachment.stencil_load_op     = GPULoadOp::LOAD;
        depth_attachment.stencil_store_op    = GPUStoreOp::DISCARD;
        depth_attachment.stencil_read_only   = true;

        // render pass info
        auto render_pass                     = GPURenderPassDescriptor{};
        render_pass.color_attachments        = {color_attachment};
        render_pass.depth_stencil_attachment = depth_attachment;

        command.begin_render_pass(render_pass);
        command.set_viewport(0, 0, desc.width, desc.height);
        command.set_scissor_rect(0, 0, desc.width, desc.height);
        command.set_pipeline(pipeline_draw);
        command.set_vertex_buffer(0, geometry.vbuffer);
        command.set_index_buffer(geometry.ibuffer, GPUIndexFormat::UINT32);
        command.set_bind_group(0, bind_group);
        command.set_stencil_reference(0x1);
        command.draw_indexed(6, 1, 0, 0, 0);
        command.end_render_pass();
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
            auto entry          = GPUBindGroupEntry{};
            entry.type          = GPUBindingResourceType::BUFFER;
            entry.binding       = 0;
            entry.buffer.buffer = uniform.ubuffer;
            entry.buffer.offset = 0;
            entry.buffer.size   = 0;

            auto desc   = GPUBindGroupDescriptor{};
            desc.layout = blayout;
            desc.entries.push_back(entry);
            return device.create_bind_group(desc);
        });

        // synchronization when window is enabled
        if (desc.window) {
            command.wait(backbuffer.available, GPUBarrierSync::PIXEL_SHADING);
            command.signal(backbuffer.complete, GPUBarrierSync::RENDER_TARGET);
        }

        // commands
        command.resource_barrier(state_transition(backbuffer.texture, undefined_state(), color_attachment_state()));
        command.resource_barrier(state_transition(dsbuffer, undefined_state(), depth_stencil_attachment_state()));
        render_mask(command, backbuffer, bind_group);
        render_color(command, backbuffer, bind_group);
        postprocessing(command, backbuffer.texture);
        command.submit();
    }
};

TEST_CASE("rhi::vulkan::stencil_test" * doctest::description("Rendering a triangle with stencil test enabled."))
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
    StencilTestApp(desc).run();
}

#ifdef WIN32
TEST_CASE("rhi::d3d12::stencil_test" * doctest::description("Rendering a triangle with stencil test enabled."))
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
    StencilTestApp(desc).run();
}
#endif
