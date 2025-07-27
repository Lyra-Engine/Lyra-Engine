#pragma once

#ifndef LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_ENUMS_H
#define LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_ENUMS_H

namespace lyra::rpi
{
    enum struct FrameGraphResourceType
    {
        TRANSIENT,
        IMPORTED,
    };

    enum struct FrameGraphReadOp
    {
        READ,
        SAMPLE,
    };

    enum struct FrameGraphWriteOp
    {
        WRITE,
        RENDER,
    };

    using FGReadOp       = FrameGraphReadOp;
    using FGWriteOp      = FrameGraphWriteOp;
    using FGResourceType = FrameGraphResourceType;

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_FRAME_GRAPH_ENUMS_H
