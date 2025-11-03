#include <Lyra/Vendor/IMGUI.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Engine/Editor/AppIcons.h>
#include <Lyra/Engine/Editor/AppColors.h>
#include <Lyra/Engine/Editor/GameView.h>
#include <Lyra/Engine/System/LayoutManager.h>

#define LYRA_GAME_WINDOW_NAME (LYRA_ICON_GAME " Game")

using namespace lyra;

GameView::GameView()
{
}

void GameView::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&GameView::update, this);
}

void GameView::update(Blackboard& blackboard)
{
    lyra::execute_once([&]() {
        auto& layout = blackboard.get<LayoutInfo>();
        ImGui::DockBuilderDockWindow(LYRA_GAME_WINDOW_NAME, layout.main);
    });

    ImGui::Begin(LYRA_GAME_WINDOW_NAME);
    ImGui::End();
}
