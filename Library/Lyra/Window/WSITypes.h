#ifndef LYRA_LIBRARY_WINDOW_TYPES_H
#define LYRA_LIBRARY_WINDOW_TYPES_H

#include <chrono>
#include <functional>

#include <Lyra/Common/Pointer.h>
#include <Lyra/Common/Container.h>
#include <Lyra/Common/Enums.h>
#include <Lyra/Window/WSIEnums.h>
#include <Lyra/Window/WSIDescs.h>
#include <Lyra/Window/WSIUtils.h>

namespace lyra::wsi
{

    using WindowCallback = std::function<void(WindowEvent)>;

    struct WindowAPI;

    struct WindowInfo
    {
        uint32_t width;        // logical window width
        uint32_t height;       // logical window height
        float    scale = 1.0f; // reserved for high DPI
    };

    struct WindowInput
    {
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

        WindowInputState states[2];

        uint state_index = 0;

        float delta_time = 0.0f; // in seconds

        TimePoint elapsed_time;

        explicit WindowInput();

        void update(const WindowHandle& handle);

        bool is_mouse_moved(MouseButton button) const;
        bool is_mouse_dragged(MouseButton button) const;
        bool is_mouse_pressed(MouseButton button) const;
        bool is_mouse_released(MouseButton button) const;

        bool is_key_down(KeyButton key) const;     // when key is pressed down and hold (simply the current status)
        bool is_key_pressed(KeyButton key) const;  // exactly once when key is pressed down
        bool is_key_released(KeyButton key) const; // exactly once when key is released up

        auto current_state() const -> const WindowInputState& { return states[state_index]; }
        auto previous_state() const -> const WindowInputState& { return states[(state_index + 1) % 2]; }
    };

    struct WindowCallbacks
    {
        Vector<std::function<void()>>                   start;
        Vector<std::function<void()>>                   close;
        Vector<std::function<void()>>                   timer;
        Vector<std::function<void(const WindowInput&)>> update;
        Vector<std::function<void()>>                   render;
        Vector<std::function<void(const WindowInfo&)>>  resize;
    };

    struct Window
    {
        friend struct EventLoop;

        static auto init(const WindowDescriptor& descriptor) -> OwnedResource<Window>;

        static auto api() -> WindowAPI*;

        // NOTE: only a convenience method for launching window and run.
        // use EventLoop::bind(...) and EventLoop::run() for multiple windows
        void loop();

        void destroy();

        auto get_window_info() const -> WindowInfo;

        template <WindowEvent E, typename F, typename T>
        void bind(F&& f, T* user)
        {
            bind<E>(std::bind(f, user, std::placeholders::_1));
        }

        template <WindowEvent E, typename F>
        void bind(F&& f)
        {
            if constexpr (E == WindowEvent::START) {
                callbacks.start.push_back(f);
            }

            if constexpr (E == WindowEvent::CLOSE) {
                callbacks.close.push_back(f);
            }

            if constexpr (E == WindowEvent::TIMER) {
                callbacks.timer.push_back(f);
            }

            if constexpr (E == WindowEvent::UPDATE) {
                callbacks.update.push_back(f);
            }

            if constexpr (E == WindowEvent::RENDER) {
                callbacks.render.push_back(f);
            }

            if constexpr (E == WindowEvent::RESIZE) {
                callbacks.resize.push_back(f);
            }
        }

    public:
        WindowHandle handle;

    private:
        WindowCallbacks callbacks;
        WindowInput     inputs;
        WindowInfo      info;
    };

    struct EventLoop
    {
        static void bind(Window& window);

        static void run();
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_TYPES_H
