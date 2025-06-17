#ifndef LYRA_LIBRARY_RENDER_RPI_ENUMS_H
#define LYRA_LIBRARY_RENDER_RPI_ENUMS_H

#include <Render/RHI/Types.h>

namespace lyra::rpi
{
    enum struct RDGResource
    {
        BUFFER,
        TEXTURE,
    };

    enum struct RDGAttachmentType
    {
        COLOR,
        COLOR_RESOLVE,
        DEPTH_STENCIL,
        DEPTH,
        STENCIL,
    };

} // namespace lyra::rpi

#endif // LYRA_LIBRARY_RENDER_RPI_ENUMS_H
