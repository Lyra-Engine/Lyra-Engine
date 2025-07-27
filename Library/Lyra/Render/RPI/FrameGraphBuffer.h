#pragma once

#ifndef LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUFFER_H
#define LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUFFER_H

#include <Lyra/Render/RHI/Descs.h>
#include <Lyra/Render/RPI/FrameGraphAllocator.h>

namespace lyra::rpi
{
    using namespace lyra::rhi;

    struct FrameGraphBuffer
    {
        using Descriptor = GPUBufferDescriptor;

        static void create(FrameGraphAllocator* allocator)
        {
            assert(!!!"Not implemented");
        }

        static void destroy(FrameGraphAllocator* allocator, const FrameGraphBuffer& buffer)
        {
            assert(!!!"Not implemented");
        }

        // related texture handles
        GPUBufferHandle buffer;
    };

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUFFER_H
