#ifndef LYRA_LIBRARY_WINDOW_EVENT_H
#define LYRA_LIBRARY_WINDOW_EVENT_H

#include <Lyra/Common/Container.h>
#include <Lyra/Window/WSIEnums.h>

namespace lyra::wsi
{
    // mouse movement / position
    struct MousePosition
    {
        float x;
        float y;
    };

    // scroll movement
    struct ScrollMovement
    {
        float x;
        float y;
    };

    // character typed event
    struct CharacterEvent
    {
        uint code;
    };

    // keyboard press/release event
    struct KeyboardEvent
    {
        KeyButton   button;
        ButtonState state;
    };

    // mouse click press/release event
    struct MouseEvent
    {
        MouseButton button;
        ButtonState state;
    };

    // unified keyboard/mouse input event
    struct InputEvent
    {
        InputEventType type;
        union
        {
            MouseEvent     mouse;
            KeyboardEvent  keyboard;
            CharacterEvent character;
        };
    };

    // a buffer of inputs within a frame,
    // used to query current frame's input
    struct InputEventQuery
    {
        static constexpr uint MAX_EVENTS = 16;

        uint                          num_events = 0;
        Array<InputEvent, MAX_EVENTS> input_events;
        MousePosition                 mouse_position;
        ScrollMovement                scroll_movement;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_EVENT_H
