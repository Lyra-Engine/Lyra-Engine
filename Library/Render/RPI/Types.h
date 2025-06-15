#ifndef LYRA_LIBRARY_RENDER_RPI_TYPES_H
#define LYRA_LIBRARY_RENDER_RPI_TYPES_H

#include <Common/Handle.h>
#include <Common/Function.h>
#include <Common/Container.h>
#include <Render/RHI/Types.h>
#include <Render/RPI/Enums.h>
#include <Render/RPI/Utils.h>

namespace lyra::rpi
{
    using namespace lyra::rhi;

    struct RDGBufferResource;
    struct RDGTextureResource;

    struct RDG
    {
        void execute();

    private:
        // resource mapping
        HashMap<RDGBufferHandle, RDGBufferResource>   buffers;
        HashMap<RDGTextureHandle, RDGTextureResource> textures;
    };

    struct RDGContext
    {
        GPUCommandBuffer command_buffer;
    };

    // abstract pass base class
    struct RDGPass
    {
        using ExecuteCallback = std::function<void(RDGContext* context)>;

        String name;

        // avoid unexpectedly being copied or moved
        explicit RDGPass(const RDGPass& other) = delete;
        explicit RDGPass(RDGPass&& other)      = delete;

        // make base class abstract
        virtual ~RDGPass() = 0;

        // reading and writing of a buffer
        void read(RDGBufferHandle buffer, GPUSize64 offset = 0, GPUSize64 size = 0);
        void write(RDGBufferHandle buffer, GPUSize64 offset = 0, GPUSize64 size = 0);

        // prepare a texture for sampling usage
        void sample(RDGTextureHandle texture);

        // presentation of a texture to swapchain
        void present(RDGTextureHandle texture);

        template <typename F>
        void execute(ExecuteCallback&& f) { callback = f; }

    protected:
        RDG*                   graph = nullptr;
        ExecuteCallback        callback;
        Vector<RDGBufferView>  buffers;
        Vector<RDGTextureView> textures;
        Vector<RDGAttachment>  attachments;
    };

    // compute pass abstraction
    struct RDGComputePass : public RDGPass
    {
        virtual ~RDGComputePass() {}
    };

    // render pass abstraction
    struct RDGRenderPass : public RDGPass
    {
        virtual ~RDGRenderPass() {}

        // attachment
        void use_color_attachment(RDGTextureHandle texture, GPULoadOp load, GPUStoreOp store = GPUStoreOp::STORE);
        void use_depth_attachment(RDGTextureHandle texture, GPULoadOp load, GPUStoreOp store = GPUStoreOp::STORE);
        void use_stencil_attachment(RDGTextureHandle texture, GPULoadOp load, GPUStoreOp store = GPUStoreOp::STORE);
        void use_depth_stencil_attachment(RDGTextureHandle texture, GPULoadOp load, GPUStoreOp store = GPUStoreOp::STORE);

        // additional settings on the clear values
        void clear_color(RDGTextureHandle texture, const GPUColor& color);
        void clear_depth(RDGTextureHandle texture, float depth);
        void clear_stencil(RDGTextureHandle texture, uint8_t stencil);
    };

    struct RDGBuilder
    {
    public:
        explicit RDGBuilder();

        auto compile() -> Own<RDG>;

        auto create_render_pass(const String& name) -> RDGRenderPass&;
        auto create_compute_pass(const String& name) -> RDGComputePass&;

        // import or deferred creation of a buffer
        auto create_buffer(GPUBufferHandle buffer) -> RDGBufferHandle;
        auto create_buffer(const GPUBufferDescriptor& desc) -> RDGBufferHandle;
        auto create_buffer(GPUSize64 size, GPUBufferUsageFlags usages) -> RDGBufferHandle;

        // import or deferred creation of a texture
        auto create_texture(GPUTextureHandle buffer) -> RDGTextureHandle;
        auto create_texture(const GPUTextureDescriptor& desc) -> RDGTextureHandle;
        auto create_texture(GPUTextureFormat format, GPUExtent2D extent) -> RDGTextureHandle;

    private:
        Own<RDG> rdg;
    };

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_TYPES_H
