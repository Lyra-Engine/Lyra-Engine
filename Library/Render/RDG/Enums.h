#ifndef LYRA_LIBRARY_RENDER_RDG_ENUMS_H
#define LYRA_LIBRARY_RENDER_RDG_ENUMS_H

#include <Render/RHI/Types.h>

namespace lyra::rdg
{
    enum struct RDGResource
    {
        BUFFER,
        TEXTURE,
    };

    enum struct RDGBufferAction
    {
        READ,
        WRITE,
    };

    enum struct RDGTextureAction
    {
        READ,
        WRITE,
        SAMPLE,
        PRESENT,
    };

    enum struct RDGAttachmentType
    {
        COLOR,
        COLOR_RESOLVE,
        DEPTH_STENCIL,
        DEPTH,
        STENCIL,
    };

} // namespace lyra::rdg

#endif // LYRA_LIBRARY_RENDER_rdg_ENUMS_H
