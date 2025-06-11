#ifndef LYRA_LIBRARY_WINDOW_UTILS_H
#define LYRA_LIBRARY_WINDOW_UTILS_H

#include <Common/Enums.h>
#include <Common/Stdint.h>
#include <Common/Container.h>
#include <Common/BitFlags.h>

#include <Window/Enums.h>

namespace lyra::wsi
{
    using WindowFlags = BitFlags<WindowFlag>;
    using Modifiers   = BitFlags<Modifier>;

    struct MousePosition
    {
        float x;
        float y;
    };

    struct MouseState
    {
        static constexpr size_t COUNT = magic_enum::enum_count<MouseButton>();

        MousePosition             position;
        Array<ButtonState, COUNT> status;
    };

    struct KeyboardState
    {
        static constexpr size_t COUNT = magic_enum::enum_count<KeyButton>();

        Modifiers                 modifiers;
        Array<ButtonState, COUNT> status;
    };

    struct WindowInputState
    {
        MouseState    mouse;
        KeyboardState keyboard;
    };

    struct WindowHandle
    {
        void* window = nullptr;
        void* native = nullptr;

        WindowInputState* state = nullptr;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_UTILS_H
