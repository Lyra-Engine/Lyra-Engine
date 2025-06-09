#ifndef LYRA_LIBRARY_WINDOW_TYPES_H
#define LYRA_LIBRARY_WINDOW_TYPES_H

#include <functional>

#include <Common/Pointer.h>
#include <Common/Container.h>
#include <Common/Enums.h>
#include <Window/Enums.h>
#include <Window/Descs.h>
#include <Window/Utils.h>

namespace lyra::wsi
{

    struct WindowAPI;

    struct WindowInfo
    {
        uint32_t width;  // logical window width
        uint32_t height; // logical window height
        float    scale;  // reserved for high DPI
    };

    struct Window
    {
        using Callback  = std::function<void()>;
        using Callbacks = Vector<Callback>;

        WindowHandle handle;

        static auto init(const WindowDescriptor& descriptor) -> OwnedResource<Window>;

        static auto api() -> WindowAPI*;

        void destroy();

        void loop();

        auto get_window_info() const -> WindowInfo;

        template <typename T, typename F>
        void bind(WindowEvent event, F&& f, T* user)
        {
            bind(event, std::bind(f, user, std::placeholders::_1));
        }

        template <typename F>
        void bind(WindowEvent event, F&& f)
        {
            callbacks.at(static_cast<uint>(event)).push_back(f);
        }

    private:
        static constexpr size_t NUM_WINDOW_EVENTS = enum_count<WindowEvent>();

        Array<Callbacks, NUM_WINDOW_EVENTS> callbacks;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_TYPES_H
