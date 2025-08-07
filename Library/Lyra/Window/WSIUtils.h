#ifndef LYRA_LIBRARY_WINDOW_UTILS_H
#define LYRA_LIBRARY_WINDOW_UTILS_H

#include <Lyra/Common/Function.h>
#include <Lyra/Common/BitFlags.h>
#include <Lyra/Window/WSIEnums.h>

namespace lyra::wsi
{
    using WindowFlags = BitFlags<WindowFlag>;

    using WindowCallback = std::function<void(WindowEvent)>;

    struct MonitorInfo
    {
        uint  monitor_pos_x;
        uint  monitor_pos_y;
        uint  monitor_width;
        uint  monitor_height;
        uint  workarea_pos_x;
        uint  workarea_pos_y;
        uint  workarea_width;
        uint  workarea_height;
        float dpi_scale_x;
        float dpi_scale_y;
    };

    struct WindowHandle
    {
        void* window = nullptr;
        void* native = nullptr;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_UTILS_H
