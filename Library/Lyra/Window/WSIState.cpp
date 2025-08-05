#include <Lyra/Window/WSIAPI.h>
#include <Lyra/Window/WSIEvent.h>
#include <Lyra/Window/WSIState.h>
#include <Lyra/Window/WSITypes.h>

using namespace lyra;
using namespace lyra::wsi;

void InputState::update(const WindowHandle& handle)
{
    InputEventQuery query{};
    Window::api()->query_input_events(handle, query);

    // update mouse position
    mouse.position = query.mouse_position;

    // loop over events and update input state
    for (uint i = 0; i < query.num_events; i++) {
        const auto& event = query.input_events.at(i);
        switch (event.type) {
            case InputEventType::MOUSE:
                mouse.status.at(static_cast<uint>(event.mouse.button)) = event.mouse.state;
                break;
            case InputEventType::KEYBOARD:
                keyboard.status.at(static_cast<uint>(event.keyboard.button)) = event.keyboard.state;
                break;
            case InputEventType::CHARACTER:
                // ignore character inputs for now
                break;
            default:
                assert(!!!"Invalid window input event type!");
        }
    }
}

WindowInput::WindowInput()
{
    elapsed_time = std::chrono::steady_clock::now();
}

void WindowInput::update(const WindowHandle& handle)
{
    states[state_index].update(handle);
    state_index = (state_index + 1) % 2;

    auto timestamp = std::chrono::steady_clock::now();
    auto duration  = timestamp - elapsed_time;
    auto ms        = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    delta_time     = static_cast<float>(ms.count()) / 1000.0f;
    elapsed_time   = timestamp;
}

bool WindowInput::is_mouse_moved(MouseButton button) const
{
    auto& curr_mouse = current_state().mouse;
    auto& prev_mouse = previous_state().mouse;
    float delta_x    = std::abs(curr_mouse.position.x - prev_mouse.position.x);
    float delta_y    = std::abs(curr_mouse.position.y - prev_mouse.position.y);
    return (prev_mouse.status[(int)button] == ButtonState::OFF) &&
           (curr_mouse.status[(int)button] == ButtonState::OFF) &&
           ((delta_x > 1) || (delta_y > 1));
}

bool WindowInput::is_mouse_dragged(MouseButton button) const
{
    auto& curr_mouse = current_state().mouse;
    auto& prev_mouse = previous_state().mouse;
    float delta_x    = std::abs(curr_mouse.position.x - prev_mouse.position.x);
    float delta_y    = std::abs(curr_mouse.position.y - prev_mouse.position.y);
    return (prev_mouse.status[(int)button] == ButtonState::ON) &&
           (curr_mouse.status[(int)button] == ButtonState::ON) &&
           ((delta_x > 1) || (delta_y > 1));
}

bool WindowInput::is_mouse_pressed(MouseButton button) const
{
    auto& curr_mouse = current_state().mouse;
    auto& prev_mouse = previous_state().mouse;
    return (prev_mouse.status[(int)button] == ButtonState::OFF) &&
           (curr_mouse.status[(int)button] == ButtonState::ON);
}

bool WindowInput::is_mouse_released(MouseButton button) const
{
    auto& curr_mouse = current_state().mouse;
    auto& prev_mouse = previous_state().mouse;
    return (prev_mouse.status[(int)button] == ButtonState::ON) &&
           (curr_mouse.status[(int)button] == ButtonState::OFF);
}

bool WindowInput::is_key_down(KeyButton key) const
{
    auto& curr_keyboard = current_state().keyboard;
    return (curr_keyboard.status[(int)key] == ButtonState::ON);
}

bool WindowInput::is_key_pressed(KeyButton key) const
{
    auto& curr_keyboard = current_state().keyboard;
    auto& prev_keyboard = previous_state().keyboard;
    return (prev_keyboard.status[(int)key] == ButtonState::OFF) &&
           (curr_keyboard.status[(int)key] == ButtonState::ON);
}

bool WindowInput::is_key_released(KeyButton key) const
{
    auto& curr_keyboard = current_state().keyboard;
    auto& prev_keyboard = previous_state().keyboard;
    return (prev_keyboard.status[(int)key] == ButtonState::ON) &&
           (curr_keyboard.status[(int)key] == ButtonState::OFF);
}
