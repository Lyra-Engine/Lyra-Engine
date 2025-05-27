#ifndef LYRA_LIBRARY_WINDOW_ENUMS_H
#define LYRA_LIBRARY_WINDOW_ENUMS_H

#include <Common/BitFlags.h>

namespace lyra::wsi
{
    enum struct WindowFlag
    {
        DECORATED  = 0x1,
        FULLSCREEN = 0x2,
    };

    enum struct WindowEvent
    {
        START,  // window opened
        CLOSE,  // window closed
        TIMER,  // fixed update internal
        UPDATE, // regular update (same rate as render)
        RENDER, // render frame
        RESIZE, // window resized
    };

    using WindowFlags = BitFlags<WindowFlag>;

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_ENUMS_H
