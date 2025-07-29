#include "./vertex.h"
#include "./geometry.h"

Geometry Geometry::create_fullscreen()
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

    // positions
    vertices.at(0).position = {-1.0f, -1.0f, 0.0f};
    vertices.at(1).position = {+1.0f, -1.0f, 0.0f};
    vertices.at(2).position = {+1.0f, +1.0f, 0.0f};
    vertices.at(3).position = {-1.0f, -1.0f, 0.0f};
    vertices.at(4).position = {-1.0f, +1.0f, 0.0f};
    vertices.at(5).position = {+1.0f, +1.0f, 0.0f};

    // normals
    vertices.at(0).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(1).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(2).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(3).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(4).normal = {0.0f, 0.0f, 1.0f};
    vertices.at(5).normal = {0.0f, 0.0f, 1.0f};

    // colors
    vertices.at(0).color = {0.0f, 0.0f, 0.0f};
    vertices.at(1).color = {1.0f, 0.0f, 0.0f};
    vertices.at(2).color = {1.0f, 1.0f, 0.0f};
    vertices.at(3).color = {0.0f, 0.0f, 0.0f};
    vertices.at(4).color = {1.0f, 0.0f, 1.0f};
    vertices.at(5).color = {1.0f, 1.0f, 0.0f};

    // uvs
    vertices.at(0).uv = {0.0f, 0.0f};
    vertices.at(1).uv = {0.0f, 1.0f};
    vertices.at(2).uv = {1.0f, 1.0f};
    vertices.at(3).uv = {0.0f, 0.0f};
    vertices.at(4).uv = {1.0f, 0.0f};
    vertices.at(5).uv = {1.0f, 1.0f};

    // indices
    indices.at(0) = 0;
    indices.at(1) = 1;
    indices.at(2) = 2;
    indices.at(3) = 3;
    indices.at(4) = 4;
    indices.at(5) = 5;

    return geom;
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
