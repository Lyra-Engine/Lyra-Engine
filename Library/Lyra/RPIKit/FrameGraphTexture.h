#pragma once

#ifndef LYRA_LIBRARY_FRAME_GRAPH_TEXTURE_H
#define LYRA_LIBRARY_FRAME_GRAPH_TEXTURE_H

#include <Lyra/Common/Hash.h>
#include <Lyra/Render/RHIDescs.h>
#include <Lyra/Render/RHIInits.h>
#include <Lyra/RPIKit/FrameGraphPass.h>
#include <Lyra/RPIKit/FrameGraphEnums.h>
#include <Lyra/RPIKit/FrameGraphContext.h>
#include <Lyra/RPIKit/FrameGraphAllocator.h>

namespace lyra
{
    struct FrameGraphTexture
    {
        using Self       = FrameGraphTexture;
        using Descriptor = GPUTextureDescriptor;

        void create(FrameGraphAllocator* allocator, const Descriptor& descriptor)
        {
            auto handle = allocator->allocate(descriptor);
            texture     = handle.first;
            view        = handle.second;
            state       = undefined_state();
            format      = descriptor.format;
            layers      = descriptor.array_layers;
            levels      = descriptor.mip_level_count;
        }

        void destroy(FrameGraphAllocator* allocator, const Descriptor& descriptor)
        {
            auto handle = std::make_pair(texture, view);
            allocator->recycle(descriptor, handle);
            texture.reset();
            view.reset();
        }

        void pre_read(FrameGraphContext* context, FrameGraphPass* pass, FrameGraphReadOp op);
        void pre_write(FrameGraphContext* context, FrameGraphPass* pass, FrameGraphWriteOp op);

        // related texture handles
        GPUTextureHandle     texture;
        GPUTextureViewHandle view;
        GPUTextureFormat     format;
        uint                 layers = 1;
        uint                 levels = 1;
        TransitionState      state  = undefined_state();
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FRAME_GRAPH_TEXTURE_H
