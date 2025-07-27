#pragma once

#ifndef LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_TEXTURE_H
#define LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_TEXTURE_H

#include <Lyra/Common/Hash.h>
#include <Lyra/Render/RHI/Descs.h>
#include <Lyra/Render/RPI/FrameGraphAllocator.h>

namespace lyra::rpi
{
    using namespace lyra::rhi;

    struct FrameGraphTexture
    {
        using Descriptor = GPUTextureDescriptor;

        static void create(FrameGraphAllocator* allocator)
        {
            assert(!!!"Not implemented");
        }

        static void destroy(FrameGraphAllocator* allocator, const FrameGraphTexture& texture)
        {
            assert(!!!"Not implemented");
        }

        // related texture handles
        GPUTextureViewHandle view;
    };

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_TEXTURE_H
