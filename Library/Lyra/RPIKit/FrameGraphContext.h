#pragma once

#ifndef LYRA_LIBRARY_FRAME_GRAPH_CONTEXT_H
#define LYRA_LIBRARY_FRAME_GRAPH_CONTEXT_H

#include <Lyra/Render/RHITypes.h>

namespace lyra
{
    struct FrameGraphContext
    {
        GPUDevice        device;
        GPUSurface       surface;
        GPUCommandBuffer cmdlist;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_FRAME_GRAPH_CONTEXT_H
