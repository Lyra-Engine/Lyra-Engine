#pragma once

#ifndef LYRA_LIBRARY_ENGINE_EDITOR_SCENE_VIEW_H
#define LYRA_LIBRARY_ENGINE_EDITOR_SCENE_VIEW_H

#include <Lyra/Vendor/IMGUI.h>
#include <Lyra/AppKit/AppTypes.h>
#include <Lyra/Engine/Common/Canvas.h>

namespace lyra
{
    struct SceneView
    {
    public:
        explicit SceneView();

        void bind(Application& app);

        void update(Blackboard& blackboard);

        void render_default(GPUCommandBuffer command);

        auto get_backbuffer() -> Backbuffer
        {
            return canvas.get_backbuffer();
        }

    private:
        ImGuiCanvas canvas;
    };
} // namespace lyra

#endif // LYRA_LIBRARY_ENGINE_EDITOR_SCENE_VIEW_H
