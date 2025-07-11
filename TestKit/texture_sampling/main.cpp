#include "helper.h"

CString texture_sampling_program = R"""(
struct VertexInput
{
    float3 position : ATTRIBUTE0;
    float2 texcoord : ATTRIBUTE1;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

struct Camera
{
    float4x4 proj;
    float4x4 view;
};

[[vk::binding(0, 0)]]
ConstantBuffer<Camera> camera : register(b0, space0);

[[vk::binding(1, 0)]]
Texture2D<float4> main_texture : register(t1, space0);

// unfortunately have to break the binding groups due to D3D12 requirement
[[vk::binding(0, 1)]]
SamplerState main_sampler : register(s0, space1);

[shader("vertex")]
VertexOutput vsmain(VertexInput input)
{
    VertexOutput output;
    output.position = mul(mul(float4(input.position, 1.0), camera.view), camera.proj);
    output.texcoord = input.texcoord;
    return output;
}

[shader("fragment")]
float4 fsmain(VertexOutput input) : SV_Target
{
    return main_texture.Sample(main_sampler, input.texcoord);
}
)""";

struct TextureSamplingApp : public TestApp
{
    Uniform            uniform;
    Geometry           geometry;
    GPUShaderModule    vshader;
    GPUShaderModule    fshader;
    GPURenderPipeline  pipeline;
    GPUPipelineLayout  playout;
    GPUBindGroupLayout blayout0;
    GPUBindGroupLayout blayout1;
    GPUBuffer          staging;
    GPUTexture         texture;
    GPUTextureView     texview;
    GPUSampler         sampler;

    explicit TextureSamplingApp(const TestAppDescriptor& desc) : TestApp(desc)
    {
        setup_buffers();
        setup_sampler();
        setup_texture();
        setup_pipeline();
    }

    void setup_buffers()
    {
        uint  width  = desc.width;
        uint  height = desc.height;
        float fovy   = 1.05;
        float aspect = float(width) / float(height);

        geometry = Geometry::create_triangle();
        uniform  = Uniform::create(fovy, aspect, glm::vec3(0.0, 0.0, 3.0));
    }

    void setup_sampler()
    {
        auto& device = RHI::get_current_device();

        sampler = execute([&] {
            auto desc           = GPUSamplerDescriptor{};
            desc.address_mode_u = GPUAddressMode::CLAMP_TO_EDGE;
            desc.address_mode_v = GPUAddressMode::CLAMP_TO_EDGE;
            desc.address_mode_w = GPUAddressMode::CLAMP_TO_EDGE;
            desc.lod_max_clamp  = 1.0;
            desc.lod_min_clamp  = 0.0;
            desc.compare_enable = false;
            desc.compare        = GPUCompareFunction::ALWAYS;
            desc.min_filter     = GPUFilterMode::NEAREST;
            desc.mag_filter     = GPUFilterMode::NEAREST;
            desc.mipmap_filter  = GPUMipmapFilterMode::NEAREST;
            desc.max_anisotropy = 0.0;
            desc.label          = "sampler";
            return device.create_sampler(desc);
        });
    }

    void setup_texture()
    {
        auto tex = SimpleTexture2D::gradient(4, 4);
        tex.upload();

        staging = tex.buffer;
        texture = tex.texture;
        texview = tex.view;
    }

    void setup_pipeline()
    {
        auto& device = RHI::get_current_device();

        auto module = execute([&]() {
            auto desc   = CompileDescriptor{};
            desc.module = "test";
            desc.path   = "test.slang";
            desc.source = texture_sampling_program;
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

        blayout0 = execute([&]() {
            auto desc = GPUBindGroupLayoutDescriptor{};

            // camera
            {
                desc.entries.push_back(GPUBindGroupLayoutEntry{});
                auto& entry                     = desc.entries.back();
                entry.type                      = GPUBindingResourceType::BUFFER;
                entry.binding                   = 0;
                entry.count                     = 1;
                entry.visibility                = GPUShaderStage::VERTEX;
                entry.buffer.type               = GPUBufferBindingType::UNIFORM;
                entry.buffer.has_dynamic_offset = false;
            }

            // texture
            {
                desc.entries.push_back(GPUBindGroupLayoutEntry{});
                auto& entry                  = desc.entries.back();
                entry.type                   = GPUBindingResourceType::TEXTURE;
                entry.binding                = 1;
                entry.count                  = 1;
                entry.visibility             = GPUShaderStage::FRAGMENT;
                entry.texture.view_dimension = GPUTextureViewDimension::x2D;
                entry.texture.multisampled   = false;
            }

            return device.create_bind_group_layout(desc);
        });

        blayout1 = execute([&]() {
            auto desc = GPUBindGroupLayoutDescriptor{};

            // sampler
            {
                desc.entries.push_back(GPUBindGroupLayoutEntry{});
                auto& entry        = desc.entries.back();
                entry.type         = GPUBindingResourceType::SAMPLER;
                entry.binding      = 0;
                entry.count        = 1;
                entry.visibility   = GPUShaderStage::FRAGMENT;
                entry.sampler.type = GPUSamplerBindingType::FILTERING;
            }

            return device.create_bind_group_layout(desc);
        });

        playout = execute([&]() {
            auto desc               = GPUPipelineLayoutDescriptor{};
            desc.bind_group_layouts = {blayout0, blayout1};
            return device.create_pipeline_layout(desc);
        });

        pipeline = execute([&]() {
            auto position            = GPUVertexAttribute{};
            position.format          = GPUVertexFormat::FLOAT32x3;
            position.offset          = offsetof(Vertex, position);
            position.shader_location = 0;

            auto texcoord            = GPUVertexAttribute{};
            texcoord.format          = GPUVertexFormat::FLOAT32x2;
            texcoord.offset          = offsetof(Vertex, uv);
            texcoord.shader_location = 1;

            auto layout         = GPUVertexBufferLayout{};
            layout.attributes   = {position, texcoord};
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
        auto bind_group0 = execute([&]() {
            auto desc   = GPUBindGroupDescriptor{};
            desc.layout = blayout0;

            // camera
            {
                desc.entries.push_back(GPUBindGroupEntry{});
                auto& entry         = desc.entries.back();
                entry.type          = GPUBindingResourceType::BUFFER;
                entry.binding       = 0;
                entry.buffer.buffer = uniform.ubuffer;
                entry.buffer.offset = 0;
                entry.buffer.size   = 0;
            }

            // texture
            {
                desc.entries.push_back(GPUBindGroupEntry{});
                auto& entry   = desc.entries.back();
                entry.type    = GPUBindingResourceType::TEXTURE;
                entry.binding = 1;
                entry.texture = texview;
            }

            return device.create_bind_group(desc);
        });

        // create bind group
        auto bind_group1 = execute([&]() {
            auto desc   = GPUBindGroupDescriptor{};
            desc.layout = blayout1;

            // sampler
            {
                desc.entries.push_back(GPUBindGroupEntry{});
                auto& entry   = desc.entries.back();
                entry.type    = GPUBindingResourceType::SAMPLER;
                entry.binding = 0;
                entry.sampler = sampler;
            }

            return device.create_bind_group(desc);
        });

        // color attachments
        auto color_attachment        = GPURenderPassColorAttachment{};
        color_attachment.clear_value = GPUColor{0.0f, 0.0f, 0.0f, 0.0f};
        color_attachment.load_op     = GPULoadOp::CLEAR;
        color_attachment.store_op    = GPUStoreOp::STORE;
        color_attachment.view        = backbuffer.view;

        // render pass info
        auto render_pass                     = GPURenderPassDescriptor{};
        render_pass.color_attachments        = {color_attachment};
        render_pass.depth_stencil_attachment = {};

        // synchronization when window is enabled
        if (desc.window) {
            command.wait(backbuffer.available, GPUBarrierSync::PIXEL_SHADING);
            command.signal(backbuffer.complete, GPUBarrierSync::RENDER_TARGET);
        }

        // commands
        command.resource_barrier(state_transition(backbuffer.texture, undefined_state(), color_attachment_state()));
        command.begin_render_pass(render_pass);
        command.set_viewport(0, 0, desc.width, desc.height);
        command.set_scissor_rect(0, 0, desc.width, desc.height);
        command.set_pipeline(pipeline);
        command.set_vertex_buffer(0, geometry.vbuffer);
        command.set_index_buffer(geometry.ibuffer, GPUIndexFormat::UINT32);
        command.set_bind_group(0, bind_group0);
        command.set_bind_group(1, bind_group1);
        command.draw_indexed(3, 1, 0, 0, 0);
        command.end_render_pass();
        postprocessing(command, backbuffer.texture);
        command.submit();
    }
};

TEST_CASE("rhi::vulkan::texture_sampling" * doctest::description("Rendering a textured triangle with the most basic graphics pipeline."))
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
    TextureSamplingApp(desc).run();
}

#ifdef WIN32
TEST_CASE("rhi::d3d12::texture_sampling" * doctest::description("Rendering a textured triangle with the most basic graphics pipeline."))
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
    TextureSamplingApp(desc).run();
}
#endif
