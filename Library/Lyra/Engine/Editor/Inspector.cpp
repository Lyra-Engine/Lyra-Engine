#include <Lyra/Vendor/IMGUI.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Editor/AppIcons.h>
#include <Lyra/Editor/AppColors.h>
#include <Lyra/Editor/Inspector.h>
#include <Lyra/Editor/LayoutManager.h>

#define LYRA_INSPECTOR_WINDOW_NAME (LYRA_ICON_INSPECTOR " Inspector")

using namespace lyra;

Inspector::Inspector()
{
}

void Inspector::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&Inspector::update, this);
}

void Inspector::update(Blackboard& blackboard)
{
    lyra::execute_once([&]() {
        auto& layout = blackboard.get<LayoutInfo>();
        ImGui::DockBuilderDockWindow(LYRA_INSPECTOR_WINDOW_NAME, layout.right);
    });

    ImGui::Begin(LYRA_INSPECTOR_WINDOW_NAME);
    ImGui::End();
}
