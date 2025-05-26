#ifndef LYRA_LIBRARY_WINDOW_TYPES_H
#define LYRA_LIBRARY_WINDOW_TYPES_H

#include <functional>

#include <Common/Container.h>
#include <Window/Enums.h>
#include <Window/Descs.h>
#include <Window/API.h>

namespace lyra
{

    struct Window
    {
        using Callback = std::function<void()>;

        WindowHandle handle;

        Array<Callback, 5> callbacks;

        explicit Window(const WindowDescriptor& descriptor);

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

        void loop()
        {
        }

        void destroy()
        {
        }
    };

} // namespace lyra

#endif // LYRA_LIBRARY_WINDOW_TYPES_H
