#pragma once

#ifndef LYRA_LIBRARY_APPKIT_CONSOLE_MANAGER_H
#define LYRA_LIBRARY_APPKIT_CONSOLE_MANAGER_H

#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    struct ConsoleManager
    {
    public:
        explicit ConsoleManager(size_t capacity);

        void bind(Application& app);

        void update(Blackboard& blackboard);
    };
} // namespace lyra

#endif // LYRA_LIBRARY_APPKIT_CONSOLE_MANAGER_H
