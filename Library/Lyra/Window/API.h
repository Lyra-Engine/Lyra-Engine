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
        // api name
        CString (*get_api_name)();

        bool (*create_window)(const WindowDescriptor& desc, WindowHandle& window);
        void (*delete_window)(WindowHandle window);

        bool (*bind_window_callback)(WindowHandle window, WindowCallback&& callback);

        bool (*get_window_size)(WindowHandle window, uint& width, uint& height);
        bool (*get_input_state)(WindowHandle window, WindowInputState& state);

        void (*run_in_loop)();
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_API_H
