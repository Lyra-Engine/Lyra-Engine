#pragma once

#ifndef LYRA_LIBRARY_APPKIT_APP_ENUMS_H
#define LYRA_LIBRARY_APPKIT_APP_ENUMS_H

#include <Lyra/Common/Stdint.h>

namespace lyra
{
    enum struct AppEvent : uint
    {
        INIT,
        DESTROY,
        RESIZE,

        UPDATE,
        UPDATE_PRE,
        UPDATE_POST,
        UPDATE_FIXED,

        UI,
        UI_PRE,
        UI_POST,

        RENDER,
        RENDER_PRE,
        RENDER_POST,
    };
} // namespace lyra

#endif // LYRA_LIBRARY_APPKIT_APP_ENUMS_H
