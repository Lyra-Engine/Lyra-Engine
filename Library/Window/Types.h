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

    struct Window
    {
        using Callback = std::function<void()>;

        WindowHandle handle;

        static auto init(const WindowDescriptor& descriptor) -> OwnedResource<Window>;

        void destroy();

        void loop();

        template <typename T, typename F>
        void bind(WindowEvent event, F&& f, T* user)
        {
            bind(event, std::bind(f, user, std::placeholders::_1));
        }

        template <typename F>
        void bind(WindowEvent event, F&& f)
        {
            callbacks.at(static_cast<uint>(event)) = f;
        }

    private:
        static constexpr size_t NUM_WINDOW_EVENTS = enum_count<WindowEvent>();

        Array<Callback, NUM_WINDOW_EVENTS> callbacks;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_TYPES_H
