#ifndef LYRA_LIBRARY_RENDER_RDG_TYPES_H
#define LYRA_LIBRARY_RENDER_RDG_TYPES_H

#include <Lyra/Common/Handle.h>
#include <Lyra/Common/Function.h>
#include <Lyra/Common/Container.h>
#include <Lyra/Render/RHI/Types.h>
#include <Lyra/Render/RHI/Inits.h>
#include <Lyra/Render/RDG/Enums.h>
#include <Lyra/Render/RDG/Utils.h>

namespace lyra::rdg
{
    using namespace lyra;
    using namespace lyra::rhi;

    struct RDGPass;
    struct RDGRenderPass;
    struct RDGComputePass;
    struct RDGBufferResource;
    struct RDGTextureResource;

    struct RDG
    {
        friend class RDGPass;
        friend class RDGBuilder;

    public:
        explicit RDG() = default;
        virtual ~RDG();

        void execute();

        auto get_buffer(RDGBufferHandle handle) -> const GPUBuffer&;

        auto get_texture(RDGTextureHandle handle) -> const GPUTexture&;

    private:
        void transition(const GPUCommandBuffer& cmdbuffer, const RDGTextureView& texture);
        void transition(const GPUCommandBuffer& cmdbuffer, const RDGAttachment& texture);

    private:
        using PassID = uint32_t;

        // build time states
        Vector<RDGPass*>           passes;
        Vector<RDGBufferResource>  buffers;
        Vector<RDGTextureResource> textures;
        Vector<PassID>             sequence;

        // execution time states
        Vector<TransitionState> texture_states;
    };

    // abstract pass base class
    struct RDGPass
    {
        friend class RDG;
        friend class RDGBuilder;

        using ExecuteCallback = std::function<void(GPUCommandBuffer& cmdbuffer, RDG& context)>;

        String label = "";

        // avoid unexpectedly being copied or moved
        explicit RDGPass(RDG* graph, const String& label);
        explicit RDGPass(const RDGPass& other) = delete;
        explicit RDGPass(RDGPass&& other)      = delete;

        // reading and writing of a buffer
        void read(RDGBufferHandle buffer, GPUSize64 offset = 0, GPUSize64 size = 0);
        void write(RDGBufferHandle buffer, GPUSize64 offset = 0, GPUSize64 size = 0);

        // prepare a texture for sampling usage
        void read(RDGTextureHandle texture, GPUBarrierSync sync = GPUBarrierSync::COMPUTE);
        void write(RDGTextureHandle texture, GPUBarrierSync sync = GPUBarrierSync::COMPUTE);
        void sample(RDGTextureHandle texture, GPUBarrierSync sync = GPUBarrierSync::PIXEL_SHADING);

        // presentation of a texture to swapchain
        void present(RDGTextureHandle texture);

        // avoid a certain resource being culled
        void preserve(RDGBufferHandle texture);
        void preserve(RDGTextureHandle texture);

        template <typename F>
        void execute(ExecuteCallback&& f) { callback = f; }

        template <typename F, typename T>
        void execute(ExecuteCallback&& f, T* user)
        {
            callback = std::bind(f, user, std::placeholders::_1);
        }

        template <typename F>
        void iter_buffer_inputs(F&& f)
        {
            for (auto& buffer : buffers)
                if (is_read_action(buffer.action))
                    f(buffer.buffer);
        }

        template <typename F>
        void iter_buffer_outputs(F&& f)
        {
            for (auto& buffer : buffers)
                if (!is_read_action(buffer.action))
                    f(buffer.buffer);
        }

        template <typename F>
        void iter_texture_inputs(F&& f)
        {
            // textures
            for (auto& texture : textures)
                if (is_read_action(texture.action))
                    f(texture.texture);
        }

        template <typename F>
        void iter_texture_outputs(F&& f)
        {
            // textures
            for (auto& texture : textures)
                if (!is_read_action(texture.action))
                    f(texture.texture);

            // attachments
            for (auto& attachment : attachments)
                f(attachment.texture);
        }

    protected:
        RDG*                   graph = nullptr;
        uint                   pid   = static_cast<uint32_t>(-1); // pass id
        ExecuteCallback        callback;
        Vector<RDGBufferView>  buffers;
        Vector<RDGTextureView> textures;
        Vector<RDGAttachment>  attachments;
    };

    // compute pass abstraction
    struct RDGComputePass : public RDGPass
    {
        explicit RDGComputePass(RDG* graph, const String& label);
        explicit RDGComputePass(const RDGComputePass& other) = delete;
        explicit RDGComputePass(RDGComputePass&& other)      = delete;
        virtual ~RDGComputePass() {}
    };

    // render pass abstraction
    struct RDGRenderPass : public RDGPass
    {
        explicit RDGRenderPass(RDG* graph, const String& label);
        explicit RDGRenderPass(const RDGRenderPass& other) = delete;
        explicit RDGRenderPass(RDGRenderPass&& other)      = delete;
        virtual ~RDGRenderPass() {}

        // attachment (by default uses GPULoadOp::LOAD)
        void use_color_attachment(RDGTextureHandle texture, GPUStoreOp store = GPUStoreOp::STORE);
        void use_depth_attachment(RDGTextureHandle texture, GPUStoreOp store = GPUStoreOp::STORE);
        void use_stencil_attachment(RDGTextureHandle texture, GPUStoreOp store = GPUStoreOp::STORE);
        void use_depth_stencil_attachment(RDGTextureHandle texture, GPUStoreOp store = GPUStoreOp::STORE);

        // additional settings on the clear values (this would overwrite GPULoadOp)
        void clear_color(RDGTextureHandle texture, const GPUColor& color);
        void clear_depth(RDGTextureHandle texture, float depth);
        void clear_stencil(RDGTextureHandle texture, uint8_t stencil);
    };

    struct RDGBuilder
    {
    public:
        explicit RDGBuilder();

        auto compile() -> Own<RDG>;

        // enable compute queue
        void enable_async_compute();

        // render/compute pass creation
        auto create_render_pass(const String& label) -> RDGRenderPass&;
        auto create_compute_pass(const String& label) -> RDGComputePass&;

        // import or deferred creation of a buffer
        auto create_buffer(const String& label, const GPUBuffer& buffer) -> RDGBufferHandle;
        auto create_buffer(const String& label, const GPUBufferDescriptor& desc) -> RDGBufferHandle;
        auto create_buffer(const String& label, GPUSize64 size, GPUBufferUsageFlags usages) -> RDGBufferHandle;

        // import or deferred creation of a texture
        auto create_texture(const String& label, const GPUTexture& texture) -> RDGTextureHandle;
        auto create_texture(const String& label, const GPUTextureDescriptor& desc) -> RDGTextureHandle;
        auto create_texture(const String& label, GPUTextureFormat format, GPUExtent2D extent) -> RDGTextureHandle;

    private:
        void prune();
        void init_relations();
        void init_refcnts();
        void init_unused();

    private:
        using PassID     = uint32_t;
        using ResourceID = uint32_t;

        Own<RDG> rdg;

        bool async_compute = false;

        // counters for handle allocation
        uint32_t rdg_pass_counter    = 0;
        uint32_t rdg_buffer_counter  = 0;
        uint32_t rdg_texture_counter = 0;

        // reference counting
        Vector<int32_t> pass_refcnts;
        Vector<int32_t> buffer_refcnts;
        Vector<int32_t> texture_refcnts;

        // unused resources
        Vector<ResourceID> unused_buffers;
        Vector<ResourceID> unused_textures;

        // relationships
        Vector<Vector<PassID>> buffer_producers;
        Vector<Vector<PassID>> texture_producers;
    };

} // namespace lyra::rdg

#endif // LYRA_LIBRARY_RENDER_rdg_TYPES_H
