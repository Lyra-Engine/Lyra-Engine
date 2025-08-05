#ifndef LYRA_LIBRARY_WINDOW_UTILS_H
#define LYRA_LIBRARY_WINDOW_UTILS_H

#include <Lyra/Common/Function.h>
#include <Lyra/Common/BitFlags.h>
#include <Lyra/Window/WSIEnums.h>

namespace lyra::wsi
{
    using WindowFlags = BitFlags<WindowFlag>;

    using WindowCallback = std::function<void(WindowEvent)>;

    struct WindowHandle
    {
        void* window = nullptr;
        void* native = nullptr;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_UTILS_H
