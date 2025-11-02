#pragma once

#ifndef LYRA_LIBRARY_EDITOR_CONSOLE_MANAGER_H
#define LYRA_LIBRARY_EDITOR_CONSOLE_MANAGER_H

#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    struct ConsoleManager
    {
    public:
        explicit ConsoleManager(size_t capacity);

        void bind(Application& app);

        void update(Blackboard& blackboard);

    private:
        void show_bar();
        void show_logs() const;

    private:
        int  log_level = 0;
        char filter[1024];
    };
} // namespace lyra

#endif // LYRA_LIBRARY_EDITOR_CONSOLE_MANAGER_H
