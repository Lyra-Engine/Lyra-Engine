#include <Lyra/Vendor/IMGUI.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Editor/AppIcons.h>
#include <Lyra/Editor/AppColors.h>
#include <Lyra/Editor/Hierarchy.h>
#include <Lyra/Editor/LayoutManager.h>

#define LYRA_HIERARCHY_WINDOW_NAME (LYRA_ICON_HIERARCHY " Hierarchy")

using namespace lyra;

Hierarchy::Hierarchy()
{
}

void Hierarchy::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&Hierarchy::update, this);
}

void Hierarchy::update(Blackboard& blackboard)
{
    lyra::execute_once([&]() {
        auto& layout = blackboard.get<LayoutInfo>();
        ImGui::DockBuilderDockWindow(LYRA_HIERARCHY_WINDOW_NAME, layout.left);
    });

    ImGui::Begin(LYRA_HIERARCHY_WINDOW_NAME);
    ImGui::End();
}
