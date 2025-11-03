#pragma once

#ifndef LYRA_LIBRARY_EDITOR_GAME_VIEW_H
#define LYRA_LIBRARY_EDITOR_GAME_VIEW_H

#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    struct GameView
    {
    public:
        explicit GameView();

        void bind(Application& app);

        void update(Blackboard& blackboard);

    private:
    };
} // namespace lyra

#endif // LYRA_LIBRARY_EDITOR_GAME_VIEW_H
