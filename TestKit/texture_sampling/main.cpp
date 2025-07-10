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

ConstantBuffer<Camera> camera;

// used to do texture sampling
Texture2D<float4> main_texture;
SamplerState      main_sampler;

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
    GPUBindGroupLayout blayout;
    GPUBuffer          staging;
    GPUTexture         texture;
    GPUTextureView     texview;
    GPUSampler         sampler;

    explicit TextureSamplingApp(const TestAppDescriptor& desc) : TestApp(desc)
    {
        setup_buffers();
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

    void setup_texture()
    {
        auto& device = RHI::get_current_device();

        uint width  = 4;
        uint height = 4;

        staging = execute([&] {
            auto desc  = GPUBufferDescriptor{};
            desc.size  = width * height * sizeof(uint32_t);
            desc.usage = GPUBufferUsage::COPY_SRC | GPUBufferUsage::MAP_WRITE;
            desc.label = "staging";
            return device.create_buffer(desc);
        });

        texture = execute([&] {
            auto desc            = GPUTextureDescriptor{};
            desc.size.width      = width;
            desc.size.height     = width;
            desc.size.depth      = 1;
            desc.array_layers    = 1;
            desc.dimension       = GPUTextureDimension::x2D;
            desc.format          = GPUTextureFormat::RGBA8UNORM;
            desc.mip_level_count = 1;
            desc.sample_count    = 1;
            desc.usage           = GPUTextureUsage::COPY_DST | GPUTextureUsage::TEXTURE_BINDING;
            desc.label           = "texture";
            return device.create_texture(desc);
        });

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

        texview = texture.create_view();

        staging.map(GPUMapMode::WRITE);
        auto content = staging.get_mapped_range<uint>();
        for (uint y = 0; y < height; y++) {
            for (uint x = 0; x < width; x++) {
                uint index        = y * width + x;
                uint red          = uint(float(y) / float(height) * 255.0) & 0xFFu;
                uint green        = uint(float(x) / float(width) * 255.0) & 0xFFu;
                uint color        = (red << 0) | (green << 8) | 0xFF000000u;
                content.at(index) = color;
            }
        }
        staging.unmap();

        auto command = execute([&]() {
            auto desc  = GPUCommandBufferDescriptor{};
            desc.queue = GPUQueueType::DEFAULT;
            return device.create_command_buffer(desc);
        });

        auto copy_src           = GPUTexelCopyBufferInfo{};
        copy_src.buffer         = staging;
        copy_src.bytes_per_row  = 0;
        copy_src.offset         = 0;
        copy_src.rows_per_image = height;

        auto copy_dst      = GPUTexelCopyTextureInfo{};
        copy_dst.texture   = texture;
        copy_dst.aspect    = GPUTextureAspect::COLOR;
        copy_dst.mip_level = 0;
        copy_dst.origin    = {0, 0, 0};

        auto copy_ext   = GPUExtent3D{};
        copy_ext.width  = width;
        copy_ext.height = height;
        copy_ext.depth  = 1;

        command.resource_barrier(state_transition(texture, undefined_state(), copy_dst_state()));
        command.copy_buffer_to_texture(copy_src, copy_dst, copy_ext);
        command.resource_barrier(state_transition(texture, copy_dst_state(), shader_resource_state(GPUBarrierSync::PIXEL_SHADING)));
        command.submit();

        device.wait();
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

        blayout = execute([&]() {
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

            // sampler
            {
                desc.entries.push_back(GPUBindGroupLayoutEntry{});
                auto& entry        = desc.entries.back();
                entry.type         = GPUBindingResourceType::SAMPLER;
                entry.binding      = 2;
                entry.count        = 1;
                entry.visibility   = GPUShaderStage::FRAGMENT;
                entry.sampler.type = GPUSamplerBindingType::FILTERING;
            }

            return device.create_bind_group_layout(desc);
        });

        playout = execute([&]() {
            auto desc               = GPUPipelineLayoutDescriptor{};
            desc.bind_group_layouts = {blayout};
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
        auto bind_group = execute([&]() {
            auto desc   = GPUBindGroupDescriptor{};
            desc.layout = blayout;

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

            // sampler
            {
                desc.entries.push_back(GPUBindGroupEntry{});
                auto& entry   = desc.entries.back();
                entry.type    = GPUBindingResourceType::SAMPLER;
                entry.binding = 2;
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
        command.set_bind_group(0, bind_group);
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

// #ifdef WIN32
// TEST_CASE("rhi::d3d12::texture_sampling" * doctest::description("Rendering a textured triangle with the most basic graphics pipeline."))
// {
//     TestAppDescriptor desc{};
//     desc.name           = "d3d12";
//     desc.window         = false;
//     desc.backend        = RHIBackend::D3D12;
//     desc.width          = 640;
//     desc.height         = 480;
//     desc.rhi_flags      = RHIFlag::DEBUG | RHIFlag::VALIDATION;
//     desc.compile_target = CompileTarget::DXIL;
//     desc.compile_flags  = CompileFlag::DEBUG;
//     TextureSamplingApp(desc).run();
// }
// #endif
