#pragma once

#ifndef LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_CONTEXT_H
#define LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_CONTEXT_H

#include <Lyra/Render/RHI/RHITypes.h>

namespace lyra::rhi
{
    using namespace lyra::rhi;

    struct FrameGraphContext
    {
        GPUDevice        device;
        GPUSurface       surface;
        GPUCommandBuffer cmdlist;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_CONTEXT_H
