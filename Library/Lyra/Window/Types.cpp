#include <Lyra/Common/Plugin.h>
#include <Lyra/Window/API.h>
#include <Lyra/Window/Types.h>

using namespace lyra;
using namespace lyra::wsi;

using WindowPlugin = Plugin<WindowAPI>;

static Own<WindowPlugin> WINDOW_PLUGIN;

OwnedResource<Window> Window::init(const WindowDescriptor& descriptor)
{
    if (WINDOW_PLUGIN.get()) {
        show_error("WSI", "Call Window::init() exactly once!");
        exit(1);
    }

    WINDOW_PLUGIN = std::make_unique<WindowPlugin>("lyra-glfw");

    OwnedResource<Window> window(new Window());
    Window::api()->create_window(descriptor, window->handle);
    return window;
}

WindowInfo Window::get_window_info() const
{
    auto info = WindowInfo{};
    Window::api()->get_window_size(this->handle, info.width, info.height);
    return info;
}

WindowAPI* Window::api()
{
    return WINDOW_PLUGIN->get_api();
}

void Window::loop()
{
    EventLoop::bind(*this);
    EventLoop::run();
}

void Window::destroy()
{
    Window::api()->delete_window(this->handle);
}

WindowInput::WindowInput()
{
    elapsed_time = std::chrono::steady_clock::now();
}

void WindowInput::update(const WindowHandle& handle)
{
    Window::api()->get_input_state(handle, states[state_index]);
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

void EventLoop::bind(Window& window)
{
    Window::api()->bind_window_callback(window.handle, [=](WindowEvent event) mutable {
        switch (event) {
            case WindowEvent::START:
                for (auto& cb : window.callbacks.start)
                    cb();
                break;
            case WindowEvent::CLOSE:
                // invoke callbacks in reverse order
                for (auto it = window.callbacks.close.rbegin(); it != window.callbacks.close.rend(); it++)
                    (*it)();
                break;
            case WindowEvent::TIMER:
                for (auto& cb : window.callbacks.timer)
                    cb();
                break;
            case WindowEvent::UPDATE:
                window.inputs.update(window.handle);
                for (auto& cb : window.callbacks.update)
                    cb(window.inputs);
                break;
            case WindowEvent::RENDER:
                for (auto& cb : window.callbacks.render)
                    cb();
                break;
            case WindowEvent::RESIZE:
                Window::api()->get_window_size(window.handle, window.info.width, window.info.height);
                for (auto& cb : window.callbacks.resize)
                    cb(window.info);
                break;
        }
    });
}

void EventLoop::run()
{
    Window::api()->run_in_loop();
}
