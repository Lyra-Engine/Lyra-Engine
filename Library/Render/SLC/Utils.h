#ifndef LYRA_LIBRARY_RENDER_SLC_UTILS_H
#define LYRA_LIBRARY_RENDER_SLC_UTILS_H

#include <Common/String.h>
#include <Common/Handle.h>
#include <Common/BitFlags.h>
#include <Common/Pointer.h>
#include <Render/SLC/Enums.h>

ENABLE_BIT_FLAGS(lyra::rhi::CompileFlag);

namespace lyra::rhi
{
    using CompileFlags = BitFlags<CompileFlag>;

    using ShaderError = CString;

    extern "C" struct ShaderBlob
    {
        uint8_t* data;
        uint     size;
    };

    struct ShaderBlobDeleter
    {
        void operator()(ShaderBlob* blob)
        {
            if (blob->data) {
                delete blob->data;
                blob->data = nullptr;
            }

            blob->size = 0;
        }
    };

    using OwnedShaderBlob = Own<ShaderBlob, ShaderBlobDeleter>;

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_UTILS_H
