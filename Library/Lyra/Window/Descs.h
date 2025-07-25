#ifndef LYRA_LIBRARY_WINDOW_DESCS_H
#define LYRA_LIBRARY_WINDOW_DESCS_H

#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/String.h>
#include <Lyra/Window/Enums.h>
#include <Lyra/Window/Utils.h>

namespace lyra::wsi
{
    struct WindowDescriptor
    {
        CString     title  = "Lyra Engine";
        uint        width  = 1920;
        uint        height = 1080;
        WindowFlags flags  = WindowFlag::DECORATED;
    };

} // namespace lyra::wsi

#endif // LYRA_LIBRARY_WINDOW_DESCS_H
