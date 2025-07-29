#pragma once

#ifndef LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUFFER_H
#define LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUFFER_H

#include <Lyra/Render/RHI/RHIDescs.h>
#include <Lyra/Render/RPI/FrameGraphAllocator.h>

namespace lyra::rhi
{
    using namespace lyra::rhi;

    struct FrameGraphBuffer
    {
        using Self       = FrameGraphBuffer;
        using Descriptor = GPUBufferDescriptor;

        void create(FrameGraphAllocator* allocator, const Descriptor& descriptor)
        {
            buffer = allocator->allocate(descriptor);
        }

        void destroy(FrameGraphAllocator* allocator, const Descriptor& descriptor)
        {
            allocator->recycle(descriptor, buffer);
            buffer.reset();
        }

        // related texture handles
        GPUBufferHandle buffer;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_BUFFER_H
