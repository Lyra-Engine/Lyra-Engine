#include <cstdint>
#include <iostream>
#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "helper.h"

Uniform Uniform::create(float fovy, float aspect, const glm::vec3& eye)
{
    Uniform res = {};

    Camera camera;
    camera.proj = glm::perspective(fovy, aspect, 0.1f, 100.0f);
    camera.view = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    auto& device = RHI::get_current_device();

    res.ubuffer = execute([&]() {
        auto desc               = GPUBufferDescriptor{};
        desc.label              = "uniform_buffer";
        desc.size               = sizeof(Camera);
        desc.usage              = GPUBufferUsage::UNIFORM | GPUBufferUsage::MAP_WRITE;
        desc.mapped_at_creation = true;
        return device.create_buffer(desc);
    });

    auto uniform  = res.ubuffer.get_mapped_range<Camera>();
    uniform.at(0) = camera;
    return res;
}

Geometry Geometry::create_triangle()
{
    Geometry geom = {};

    auto& device = RHI::get_current_device();

    geom.vbuffer = execute([&]() {
        auto desc               = GPUBufferDescriptor{};
        desc.label              = "vertex_buffer";
        desc.size               = sizeof(Vertex) * 3;
        desc.usage              = GPUBufferUsage::VERTEX | GPUBufferUsage::MAP_WRITE;
        desc.mapped_at_creation = true;
        return device.create_buffer(desc);
    });

    geom.ibuffer = execute([&]() {
        auto desc               = GPUBufferDescriptor{};
        desc.label              = "index_buffer";
        desc.size               = sizeof(uint32_t) * 3;
        desc.usage              = GPUBufferUsage::INDEX | GPUBufferUsage::MAP_WRITE;
        desc.mapped_at_creation = true;
        return device.create_buffer(desc);
    });

    auto vertices = geom.vbuffer.get_mapped_range<Vertex>();
    auto indices  = geom.ibuffer.get_mapped_range<uint>();

    // positions
    vertices.at(0).position = {0.0f, 0.0f, 0.0f};
    vertices.at(1).position = {1.0f, 0.0f, 0.0f};
    vertices.at(2).position = {0.0f, 1.0f, 0.0f};

    // normals
    vertices.at(0).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(1).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(2).normal = {0.0f, 0.0f, 1.0f};

    // colors
    vertices.at(0).color = {1.0f, 0.0f, 0.0f};
    vertices.at(1).color = {0.0f, 1.0f, 0.0f};
    vertices.at(2).color = {0.0f, 0.0f, 1.0f};

    // uvs
    vertices.at(0).uv = {0.0f, 0.0f};
    vertices.at(1).uv = {0.0f, 1.0f};
    vertices.at(2).uv = {1.0f, 0.0f};

    // indices
    indices.at(0) = 0;
    indices.at(1) = 1;
    indices.at(2) = 2;

    return geom;
}

Geometry Geometry::create_overlapping_triangles()
{
    Geometry geom = {};

    auto& device = RHI::get_current_device();

    geom.vbuffer = execute([&]() {
        auto desc               = GPUBufferDescriptor{};
        desc.label              = "vertex_buffer";
        desc.size               = sizeof(Vertex) * 6;
        desc.usage              = GPUBufferUsage::VERTEX | GPUBufferUsage::MAP_WRITE;
        desc.mapped_at_creation = true;
        return device.create_buffer(desc);
    });

    geom.ibuffer = execute([&]() {
        auto desc               = GPUBufferDescriptor{};
        desc.label              = "index_buffer";
        desc.size               = sizeof(uint32_t) * 6;
        desc.usage              = GPUBufferUsage::INDEX | GPUBufferUsage::MAP_WRITE;
        desc.mapped_at_creation = true;
        return device.create_buffer(desc);
    });

    auto vertices = geom.vbuffer.get_mapped_range<Vertex>();
    auto indices  = geom.ibuffer.get_mapped_range<uint>();

    // positions (tri 1)
    vertices.at(0).position = {0.0f, 0.0f, 0.0f};
    vertices.at(1).position = {1.0f, 0.0f, 0.0f};
    vertices.at(2).position = {0.0f, 1.0f, 0.0f};

    // normals (tri 1)
    vertices.at(0).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(1).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(2).normal = {0.0f, 0.0f, 1.0f};

    // colors (tri 1)
    vertices.at(0).color = {1.0f, 0.0f, 1.0f};
    vertices.at(1).color = {1.0f, 0.0f, 1.0f};
    vertices.at(2).color = {1.0f, 0.0f, 1.0f};

    // uvs (tri 1)
    vertices.at(0).uv = {0.0f, 0.0f};
    vertices.at(1).uv = {0.0f, 1.0f};
    vertices.at(2).uv = {1.0f, 0.0f};

    // positions (tri 2)
    vertices.at(3).position = {0.0f - 0.25f, 0.0f - 0.25f, -1.0f};
    vertices.at(4).position = {1.0f - 0.25f, 0.0f - 0.25f, -1.0f};
    vertices.at(5).position = {0.0f - 0.25f, 1.0f - 0.25f, -1.0f};

    // normals (tri 2)
    vertices.at(3).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(4).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(5).normal = {0.0f, 0.0f, 1.0f};

    // colors (tri 2)
    vertices.at(3).color = {0.0f, 1.0f, 1.0f};
    vertices.at(4).color = {0.0f, 1.0f, 1.0f};
    vertices.at(5).color = {0.0f, 1.0f, 1.0f};

    // uvs (tri 2)
    vertices.at(3).uv = {0.0f, 0.0f};
    vertices.at(4).uv = {0.0f, 1.0f};
    vertices.at(5).uv = {1.0f, 0.0f};

    // indices
    indices.at(0) = 0;
    indices.at(1) = 1;
    indices.at(2) = 2;
    indices.at(3) = 3;
    indices.at(4) = 4;
    indices.at(5) = 5;

    return geom;
}

GPUTexelCopyTextureInfo RenderTarget::copy_src() const
{
    auto texture_region      = GPUTexelCopyTextureInfo{};
    texture_region.texture   = texture;
    texture_region.aspect    = GPUTextureAspect::COLOR;
    texture_region.mip_level = 0;
    texture_region.origin    = GPUOrigin3D{0, 0, 0};
    return texture_region;
}

GPUTexelCopyBufferInfo RenderTarget::copy_dst() const
{
    auto buffer_region           = GPUTexelCopyBufferInfo{};
    buffer_region.buffer         = buffer;
    buffer_region.offset         = 0;
    buffer_region.bytes_per_row  = 0; // NOTE: currently let underlying implemetation decide
    buffer_region.rows_per_image = height;
    return buffer_region;
}

GPUExtent3D RenderTarget::copy_ext() const
{
    return GPUExtent3D{width, height, 1};
}

void RenderTarget::save(CString filename)
{
#if 0
    auto cwd = std::filesystem::current_path();
    std::cout << "cwd: " << cwd << std::endl;
#endif

    buffer.map(GPUMapMode::READ);
    {
        auto data = buffer.get_mapped_range<uint8_t>();
        stbi_write_png(filename, width, height, 4, data.data, width * 4);
    }
    buffer.unmap();
}

RenderTarget RenderTarget::create(GPUTextureFormat format, uint width, uint height, uint samples)
{
    RenderTarget res = {};

    GPUTextureDescriptor tex_desc{};
    tex_desc.label           = "render target";
    tex_desc.format          = format;
    tex_desc.dimension       = GPUTextureDimension::x2D;
    tex_desc.size.width      = width;
    tex_desc.size.height     = height;
    tex_desc.size.depth      = 1;
    tex_desc.array_layers    = 1;
    tex_desc.mip_level_count = 1;
    tex_desc.sample_count    = samples;
    tex_desc.usage           = GPUTextureUsage::COPY_SRC | GPUTextureUsage::RENDER_ATTACHMENT;

    GPUBufferDescriptor buf_desc{};
    buf_desc.label = "host backbuffer";
    buf_desc.usage = GPUBufferUsage::COPY_DST | GPUBufferUsage::MAP_READ;
    buf_desc.size  = sizeof(uint32_t) * width * height;

    auto& device = RHI::get_current_device();
    res.format   = tex_desc.format;
    res.width    = width;
    res.height   = height;
    res.buffer   = device.create_buffer(buf_desc);
    res.texture  = device.create_texture(tex_desc);
    res.view     = res.texture.create_view();
    return res;
}

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
