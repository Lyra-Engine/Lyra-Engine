#pragma once

#ifndef LYRA_LIBRARY_ENGINE_SYSTEM_THEME_MANAGER_H
#define LYRA_LIBRARY_ENGINE_SYSTEM_THEME_MANAGER_H

#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    struct ThemeManager
    {
    public:
        explicit ThemeManager() = default;

        void bind(Application& app);

        void config();
    };
} // namespace lyra

#endif // LYRA_LIBRARY_ENGINE_SYSTEM_THEME_MANAGER_H
