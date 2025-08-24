#ifndef LYRA_LIBRARY_WINDOW_EVENT_H
#define LYRA_LIBRARY_WINDOW_EVENT_H

#include <Lyra/Common/Container.h>
#include <Lyra/Window/WSIEnums.h>

namespace lyra
{
    // character typed event
    struct KeyTypingEvent
    {
        uint code;
    };

    // keyboard press/release event
    struct KeyButtonEvent
    {
        KeyButton   button;
        ButtonState state;
    };

    // wheel movement
    struct MouseWheelEvent
    {
        float x;
        float y;
    };

    // mouse movement / position
    struct MouseMoveEvent
    {
        float xpos;
        float ypos;
    };

    // mouse click press/release event
    struct MouseButtonEvent
    {
        MouseButton button;
        ButtonState state;
    };

    // window movement event
    struct WindowMoveEvent
    {
        uint xpos;
        uint ypos;
    };

    // window focus event
    struct WindowFocusEvent
    {
        bool focused;
    };

    // window close event
    struct WindowCloseEvent
    {
        // no additional information
    };

    // window resize event
    struct WindowResizeEvent
    {
        uint width;
        uint height;
    };

    // unified keyboard/mouse input event
    struct InputEvent
    {
        enum struct Type : uint
        {
            KEY_TYPING,
            KEY_BUTTON,
            MOUSE_MOVE,
            MOUSE_WHEEL,
            MOUSE_BUTTON,
            WINDOW_MOVE,
            WINDOW_FOCUS,
            WINDOW_CLOSE,
            WINDOW_RESIZE,
        } type;

        union
        {
            KeyTypingEvent    key_typing;
            KeyButtonEvent    key_button;
            MouseMoveEvent    mouse_move;
            MouseWheelEvent   mouse_wheel;
            MouseButtonEvent  mouse_button;
            WindowMoveEvent   window_move;
            WindowFocusEvent  window_focus;
            WindowCloseEvent  window_close;
            WindowResizeEvent window_resize;
        };
    };

    // a buffer of inputs within a frame,
    // used to query current frame's input
    struct InputEventQuery
    {
        static constexpr uint MAX_EVENTS = 16;

        uint                          num_events = 0;
        Array<InputEvent, MAX_EVENTS> input_events;
    };

} // namespace lyra

#endif // LYRA_LIBRARY_WINDOW_EVENT_H
