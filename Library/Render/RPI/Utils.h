#ifndef LYRA_LIBRARY_RENDER_RPI_UTILS_H
#define LYRA_LIBRARY_RENDER_RPI_UTILS_H

#include <Render/RHI/Types.h>
#include <Render/RPI/Enums.h>

namespace lyra::rpi
{
    using namespace lyra::rhi;

    using RDGBufferHandle  = Handle<RDGResource, RDGResource::BUFFER>;
    using RDGTextureHandle = Handle<RDGResource, RDGResource::TEXTURE>;

    // Users are supposed to only use RDGBufferHandle.
    // RDGBufferResource is supposed to be only used internally.
    struct RDGBufferResource
    {
        GPUBufferHandle     buffer;
        GPUBufferDescriptor descriptor;
    };

    // Users are supposed to only use RDGTextureHandle.
    // RDGTextureResource is supposed to be only used internally.
    struct RDGTextureResource
    {
        GPUTextureHandle     texture;
        GPUTextureDescriptor descriptor;
    };

    struct RDGBufferView
    {
        RDGBufferHandle buffer;
        GPUSize64       offset = 0;
        GPUSize64       size   = 0;
    };

    struct RDGTextureView
    {
        RDGTextureHandle     texture;
        GPUIntegerCoordinate base_mip_level    = 0;
        GPUIntegerCoordinate mip_level_count   = 1;
        GPUIntegerCoordinate base_array_layer  = 0;
        GPUIntegerCoordinate array_layer_count = 1;
    };

    // Users are not supposed to directly use RDGAttachment.
    // RDGAttachment is a helper struct used by RDGPass.
    struct RDGAttachment
    {
        RDGTextureHandle texture;
        GPULoadOp        load_op  = GPULoadOp::LOAD;
        GPUStoreOp       store_op = GPUStoreOp::STORE;
        union
        {
            GPUColor clear_color = GPUColor{0.0f, 0.0f, 0.0f, 0.0f};
            float    clear_depth;
            uint8_t  clear_stencil;
        };
    };

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_UTILS_H
