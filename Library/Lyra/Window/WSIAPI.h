#ifndef LYRA_LIBRARY_WINDOW_API_H
#define LYRA_LIBRARY_WINDOW_API_H

#include <Lyra/Window/WSIEnums.h>
#include <Lyra/Window/WSIUtils.h>
#include <Lyra/Window/WSIDescs.h>
#include <Lyra/Window/WSITypes.h>

namespace lyra::wsi
{
    struct WindowAPI
    {
        // api name
        CString (*get_api_name)();

        bool (*create_window)(const WindowDescriptor& desc, WindowHandle& window);
        void (*delete_window)(WindowHandle window);

        void (*set_window_pos)(WindowHandle window, uint x, uint y);
        void (*get_window_pos)(WindowHandle window, uint& x, uint& y);

        void (*set_window_size)(WindowHandle window, uint width, uint height);
        void (*get_window_size)(WindowHandle window, uint& width, uint& height);

        void (*get_window_scale)(WindowHandle window, float& xscale, float& yscale);

        void (*set_window_focus)(WindowHandle window);
        bool (*get_window_focus)(WindowHandle window);

        void (*set_window_alpha)(WindowHandle window, float alpha);
        float (*get_window_alpha)(WindowHandle window);

        void (*set_window_title)(WindowHandle window, CString title);
        CString (*get_window_title)(WindowHandle window);

        bool (*get_window_minimized)(WindowHandle window);
        void (*get_input_state)(WindowHandle window, WindowInputState& state);

        void (*bind_window_callback)(WindowHandle window, WindowCallback&& callback);

        void (*show_window)(WindowHandle window);
        void (*swap_buffer)(WindowHandle window);

        void (*run_in_loop)();
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_API_H
