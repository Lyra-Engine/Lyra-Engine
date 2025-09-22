#include <Lyra/Common/Logger.h>
#include <Lyra/AppKit/Builtin/LayoutManager.h>
#include <Lyra/AppKit/Builtin/ConsoleManager.h>

#include <imgui.h>
#include <imgui_internal.h>

using namespace lyra;

ConsoleManager::ConsoleManager(size_t capacity)
{
    get_console_sink()->get_console().resize(capacity);
}

void ConsoleManager::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&ConsoleManager::update, this);
}

void ConsoleManager::update(Blackboard& blackboard)
{
    auto& layout = blackboard.get<LayoutInfo>();
    ImGui::DockBuilderDockWindow("Console", layout.bottom);

    ImGui::Begin("Console");
    ImGui::BeginChild("Console Log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    auto& sink = get_console_sink()->get_console();
    sink.for_each([&](const ConsoleLog& log) {
        ImGui::Text("%s", log.payload.c_str());
    });
    if (sink.modified()) {
        sink.reset();
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::End();
}
