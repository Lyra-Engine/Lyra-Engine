#ifndef LYRA_LIBRARY_WINDOW_TYPES_H
#define LYRA_LIBRARY_WINDOW_TYPES_H

#include <functional>

#include <Common/Container.h>
#include <Window/Enums.h>
#include <Window/Descs.h>
#include <Window/Utils.h>

namespace lyra::wsi
{

    struct Window
    {
        using Callback = std::function<void()>;

        WindowHandle handle;

        static auto init(const WindowDescriptor& descriptor) -> Window;

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
        Array<Callback, 6> callbacks;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_TYPES_H
