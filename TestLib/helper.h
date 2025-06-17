#ifndef LYRA_TESTLIB_HELPER_H
#define LYRA_TESTLIB_HELPER_H

#define DOCTEST_CONFIG_NO_EXCEPTIONS
#include <doctest/doctest.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Common/Container.h>
#include <Common/Function.h>
#include <Render/Render.hpp>

using namespace lyra;
using namespace lyra::wsi;
using namespace lyra::rhi;

struct Camera
{
    glm::mat4 proj;
    glm::mat4 view;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Uniform
{
    GPUBuffer ubuffer;

    static auto create(float fovy, float aspect, const glm::vec3& eye) -> Uniform;
};

struct Geometry
{
    GPUBuffer vbuffer;
    GPUBuffer ibuffer;

    static auto create_triangle() -> Geometry;
};

struct RenderTarget
{
    uint             width;
    uint             height;
    GPUTextureFormat format;
    GPUTexture       texture;
    GPUTextureView   view;
    GPUBuffer        buffer;

    auto copy_src() const -> GPUTexelCopyTextureInfo;
    auto copy_dst() const -> GPUTexelCopyBufferInfo;
    auto copy_ext() const -> GPUExtent3D;
    void save(CString name);

    static auto create(GPUTextureFormat format, uint width, uint height, uint samples = 1) -> RenderTarget;
};

struct TestAppDescriptor
{
    CString       name           = "";
    bool          window         = false;
    uint          width          = 960;
    uint          height         = 480;
    RHIBackend    backend        = RHIBackend::VULKAN;
    RHIFlags      rhi_flags      = RHIFlag::DEBUG | RHIFlag::VALIDATION;
    CompileTarget compile_target = CompileTarget::SPIRV;
    CompileFlags  compile_flags  = CompileFlag::DEBUG;
};

struct TestApp
{
    explicit TestApp(const TestAppDescriptor& desc);

    virtual void render(const GPUSurfaceTexture& texture) = 0;

    void run();
    void run_with_window();
    void run_without_window();
    void postprocessing(const GPUCommandBuffer& cmd, GPUTextureHandle backbuffer);
    auto get_backbuffer_format() const -> GPUTextureFormat;

    TestAppDescriptor       desc;
    OwnedResource<Window>   win;
    OwnedResource<RHI>      rhi;
    OwnedResource<Compiler> compiler;
    RenderTarget            render_target;
};

#endif // LYRA_TESTLIB_HELPER_H
