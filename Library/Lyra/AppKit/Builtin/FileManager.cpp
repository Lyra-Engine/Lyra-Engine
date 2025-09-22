#include <Lyra/Common/Path.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/AppKit/AppIcons.h>
#include <Lyra/AppKit/Builtin/FileManager.h>
#include <Lyra/AppKit/Builtin/LayoutManager.h>

#include <imgui.h>
#include <imgui_internal.h>

#define LYRA_FILES_WINDOW_NAME (LYRA_ICON_FOLDER " Files")

using namespace lyra;

FileManager::FileManager(const Path& root)
    : root(root), curr(root)
{
    // sanity check
    assert(std::filesystem::exists(root));
    assert(std::filesystem::is_directory(root));
}

void FileManager::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&FileManager::update, this);
}

void FileManager::update(Blackboard& blackboard)
{
    lyra::execute_once([&]() {
        auto& layout = blackboard.get<LayoutInfo>();
        ImGui::DockBuilderDockWindow(LYRA_FILES_WINDOW_NAME, layout.bottom);
    });

    ImGui::Begin(LYRA_FILES_WINDOW_NAME);
    show_breadcrumb();
    ImGui::Separator();
    ImGui::BeginChild("##FileBrowser");
    {
        show_dir_files();
    }
    ImGui::EndChild();
    ImGui::End();
}

void FileManager::show_breadcrumb()
{
    // root / home
    if (ImGui::Button(LYRA_ICON_HOME " Home")) {
        curr = root;
    }
    ImGui::SameLine();
    ImGui::TextUnformatted("/");
    ImGui::SameLine();

    // currently at root
    if (root == curr) {
        ImGui::NewLine();
        return;
    }

    // show directory path segments
    auto relative = std::filesystem::relative(curr, root);
    for (auto& part : relative) {
        auto parts = to_string(part);
        if (ImGui::Button(parts.c_str())) {
            curr = root;
            for (auto p : relative) {
                curr /= p;
                if (p == part)
                    break;
            }
        }
        ImGui::SameLine();
        ImGui::TextUnformatted("/");
        ImGui::SameLine();
    }
    ImGui::NewLine();
}

void FileManager::show_dir_files()
{
    // drawing grid
    int   grid_id   = 0;
    float start_x   = ImGui::GetCursorPosX();
    float row_width = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    for (const auto& entry : std::filesystem::directory_iterator(curr)) {
        const auto& path = entry.path();
        const auto  rela = to_string(std::filesystem::relative(path, curr));

        // not showing the hidden files
        if (rela.at(0) == '.') continue;

        // draw the file icon
        ImGui::PushID(grid_id++);

        ImGui::BeginGroup(); // group icon + text together
        {
            // icon
            if (entry.is_directory()) {
                // update path when clicking on the icon
                ImGui::SetWindowFontScale(3.0f);
                ImGui::Button(LYRA_ICON_FOLDER, ImVec2(icon_size, icon_size));
                ImGui::SetWindowFontScale(1.0f);
            } else {
                ImGui::SetWindowFontScale(2.0f);
                ImGui::Button(LYRA_ICON_FILE, ImVec2(icon_size, icon_size));
                ImGui::SetWindowFontScale(1.0f);
            }

            // filename text under icon
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + icon_size);
            ImGui::TextWrapped("%s", rela.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::EndGroup();

        // handle clicks
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (entry.is_directory()) {
                curr = path;
            }
        }

        // calculate next position
        float last_x = ImGui::GetItemRectMax().x;
        float next_x = last_x + padding + icon_size;
        if (next_x < row_width) {
            ImGui::SameLine();
        } else {
            ImGui::NewLine();
            ImGui::SetCursorPosX(start_x); // align new row
        }

        ImGui::PopID();
    }
    ImGui::NewLine();
}
