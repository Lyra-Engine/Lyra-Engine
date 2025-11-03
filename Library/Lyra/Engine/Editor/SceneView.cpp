#include <Lyra/Vendor/IMGUI.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Engine/Editor/AppIcons.h>
#include <Lyra/Engine/Editor/AppColors.h>
#include <Lyra/Engine/Editor/SceneView.h>
#include <Lyra/Engine/System/LayoutManager.h>

#define LYRA_SCENE_WINDOW_NAME (LYRA_ICON_SCENE " Scene")

using namespace lyra;

SceneView::SceneView()
{
}

void SceneView::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&SceneView::update, this);
}

void SceneView::update(Blackboard& blackboard)
{
    lyra::execute_once([&]() {
        auto& layout = blackboard.get<LayoutInfo>();
        ImGui::DockBuilderDockWindow(LYRA_SCENE_WINDOW_NAME, layout.main);
    });

    ImGui::Begin(LYRA_SCENE_WINDOW_NAME);
    ImGui::End();
}
