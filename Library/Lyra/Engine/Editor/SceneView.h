#pragma once

#ifndef LYRA_LIBRARY_EDITOR_SCENE_VIEW_H
#define LYRA_LIBRARY_EDITOR_SCENE_VIEW_H

#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    struct SceneView
    {
    public:
        explicit SceneView();

        void bind(Application& app);

        void update(Blackboard& blackboard);

    private:
    };
} // namespace lyra

#endif // LYRA_LIBRARY_EDITOR_SCENE_VIEW_H
