#pragma once

#ifndef LYRA_LIBRARY_AMS_TEXTURE_ASSET_H
#define LYRA_LIBRARY_AMS_TEXTURE_ASSET_H

#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/Container.h>
#include <Lyra/Render/RHIEnums.h>

namespace lyra
{
    struct TextureAsset
    {
        struct Subresource
        {
            uint offset; // offset into the binary blob
            uint pitch;  // bytes per row
            uint width;
            uint height;
        };

        GPUTextureFormat    format;
        Vector<uint8_t>     binary;
        Vector<Subresource> subresources;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_AMS_TEXTURE_H
