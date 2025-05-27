#ifndef LYRA_LIBRARY_WINDOW_UTILS_H
#define LYRA_LIBRARY_WINDOW_UTILS_H

#include <Window/Enums.h>

namespace lyra::wsi
{
    struct WindowHandle
    {
        void* window = nullptr;
        void* native = nullptr;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_UTILS_H
