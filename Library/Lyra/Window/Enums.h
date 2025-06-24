#ifndef LYRA_LIBRARY_WINDOW_ENUMS_H
#define LYRA_LIBRARY_WINDOW_ENUMS_H

#include <Lyra/Common/Stdint.h>

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

    enum struct Modifier : uint
    {
        ALT   = 0x0001,
        CTRL  = 0x0002,
        SHIFT = 0x0004,
    };

    enum struct ButtonState
    {
        OFF,
        ON,
    };

    enum struct MouseButton
    {
        LEFT,
        RIGHT,
        MIDDLE,
    };

    // clang-format off
    enum struct KeyButton
    {
        // misc keys
        TAB, ESC, SPACE, BACKSPACE, DEL, ENTER, PAGE_UP, PAGE_DOWN, HOME, END,

        // ASCII keys
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // arrow keys
        UP, DOWN, LEFT, RIGHT,

        // numeric keys
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,

        // functional keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    };
    // clang-format on

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_ENUMS_H
