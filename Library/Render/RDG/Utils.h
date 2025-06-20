#ifndef LYRA_LIBRARY_RENDER_RDG_UTILS_H
#define LYRA_LIBRARY_RENDER_RDG_UTILS_H

#include <Render/RHI/Types.h>
#include <Render/RDG/Enums.h>

namespace lyra::rdg
{
    using namespace lyra::rhi;

    using RDGBufferHandle  = Handle<RDGResource, RDGResource::BUFFER>;
    using RDGTextureHandle = Handle<RDGResource, RDGResource::TEXTURE>;

    struct RDGResourceBase
    {
        String label     = "";
        uint   rid       = static_cast<uint>(-1); // resource id
        bool   imported  = false;
        bool   preserved = false;
    };

    // Users are supposed to only use RDGBufferHandle.
    // RDGBufferResource is supposed to be only used internally.
    struct RDGBufferResource : public RDGResourceBase
    {
        GPUBuffer           buffer;
        GPUBufferDescriptor descriptor;
    };

    // Users are supposed to only use RDGTextureHandle.
    // RDGTextureResource is supposed to be only used internally.
    struct RDGTextureResource : public RDGResourceBase
    {
        GPUTexture           texture;
        GPUTextureDescriptor descriptor;
    };

    struct RDGBufferView
    {
        RDGBufferAction action;
        RDGBufferHandle buffer;
        GPUSize64       offset = 0;
        GPUSize64       size   = 0;
    };

    struct RDGTextureView
    {
        RDGTextureAction action;
        RDGTextureHandle texture;
        GPUBarrierSync   sync = GPUBarrierSync::ALL_SHADING;
    };

    // Users are not supposed to directly use RDGAttachment.
    // RDGAttachment is a helper struct used by RDGPass.
    struct RDGAttachment
    {
        RDGAttachmentType type;
        RDGTextureHandle  texture;
        GPULoadOp         load_op  = GPULoadOp::LOAD;
        GPUStoreOp        store_op = GPUStoreOp::STORE;
        union
        {
            GPUColor clear_color = GPUColor{0.0f, 0.0f, 0.0f, 0.0f};
            struct
            {
                float   clear_depth;
                uint8_t clear_stencil;
            };
        };
    };

    inline bool is_read_action(RDGBufferAction action)
    {
        switch (action) {
            case RDGBufferAction::READ:
                return true;
            case RDGBufferAction::WRITE:
            default:
                return false;
        }
    }

    inline bool is_read_action(RDGTextureAction action)
    {
        switch (action) {
            case RDGTextureAction::READ:
            case RDGTextureAction::SAMPLE:
                return true;
            case RDGTextureAction::WRITE:
            case RDGTextureAction::PRESENT:
                return false;
        }
    }

} // namespace lyra::rdg

#endif // LYRA_LIBRARY_RENDER_rdg_UTILS_H
