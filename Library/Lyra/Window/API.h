#ifndef LYRA_LIBRARY_WINDOW_API_H
#define LYRA_LIBRARY_WINDOW_API_H

#include <Lyra/Window/Enums.h>
#include <Lyra/Window/Utils.h>
#include <Lyra/Window/Descs.h>
#include <Lyra/Window/Types.h>

namespace lyra::wsi
{
    struct WindowAPI
    {
        using MainLoopCallback = std::function<void(WindowEvent)>;

        // api name
        CString (*get_api_name)();

        void (*get_window_size)(WindowHandle handle, uint& width, uint& height);
        void (*get_input_state)(WindowHandle handle, WindowInputState& state);

        bool (*create_window)(const WindowDescriptor& desc, WindowHandle& window);
        void (*delete_window)(WindowHandle window);

        void (*run_in_loop)(WindowHandle window, MainLoopCallback&& callback);
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_API_H
