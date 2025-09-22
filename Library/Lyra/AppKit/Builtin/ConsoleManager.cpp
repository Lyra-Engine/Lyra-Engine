#include <Lyra/Common/Logger.h>
#include <Lyra/AppKit/AppIcons.h>
#include <Lyra/AppKit/Builtin/LayoutManager.h>
#include <Lyra/AppKit/Builtin/ConsoleManager.h>

#include <imgui.h>
#include <imgui_internal.h>

#define LYRA_CONSOLE_WINDOW_NAME (LYRA_ICON_CONSOLE " Console")

using namespace lyra;

ConsoleManager::ConsoleManager(size_t capacity)
{
    get_console_sink()->get_console().resize(capacity);

    filter[0] = '\0';
}

void ConsoleManager::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&ConsoleManager::update, this);
}

void ConsoleManager::update(Blackboard& blackboard)
{
    lyra::execute_once([&]() {
        auto& layout = blackboard.get<LayoutInfo>();
        ImGui::DockBuilderDockWindow(LYRA_CONSOLE_WINDOW_NAME, layout.bottom);
    });

    ImGui::Begin(LYRA_CONSOLE_WINDOW_NAME);
    show_bar();
    show_logs();
    ImGui::End();
}

void ConsoleManager::show_bar()
{
    // log verbosity
    ImGui::PushItemWidth(200);
    CString log_levels[]  = {"trace", "debug", "info", "warn", "error", "critical", "off"};
    CString preview_value = log_levels[log_level];

    // just to apply some padding
    ImGui::BeginDisabled();
    ImGui::Button(LYRA_ICON_FILTER);
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::BeginCombo("##LogLevel", preview_value)) {
        for (int i = 0; i < IM_ARRAYSIZE(log_levels); i++) {
            const bool is_log_level = (log_level == i);
            if (ImGui::Selectable(log_levels[i], is_log_level)) {
                log_level = i;
            }
            if (is_log_level) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // log filter
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##LogFilter", filter, 1024, 0);
    ImGui::PopItemWidth();
}

void ConsoleManager::show_logs() const
{
    String filter_text(filter);

    ImGui::BeginChild("Console Log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    {
        auto& sink = get_console_sink()->get_console();
        sink.for_each([&](const ConsoleLog& log) {
            if (log_level <= static_cast<int>(log.verbosity))
                if (filter_text.empty() || log.payload.find(filter_text) != String::npos)
                    ImGui::Text("%s", log.payload.c_str());
        });
        if (sink.modified()) {
            sink.reset();
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
}
