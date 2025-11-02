#include <Lyra/Vendor/IMGUI.h>
#include <Lyra/Common/Path.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/AppKit/AppIcons.h>
#include <Lyra/AppKit/AppColors.h>
#include <Lyra/AppKit/Builtin/FileManager.h>
#include <Lyra/AppKit/Builtin/LayoutManager.h>

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
        show_context_menu();
        show_new_file_dialog();
        show_new_folder_dialog();
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

    // TODO: don't call listdir on every frame, only re-calculate when path is changed (dirty).
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

            const int width = ImGui::CalcTextSize(rela.c_str()).x;
            const int start = icon_size > width ? (icon_size - width) / 2 : 0;

            // filename text under icon
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start);
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

void FileManager::show_context_menu()
{
    // capture the whole file browser region for right click
    auto region = ImGui::GetContentRegionAvail();
    ImGui::InvisibleButton("##FileBrowserContext", region);

    // detect context menu (right click)
    if (ImGui::BeginPopupContextItem("File Manager Context Menu")) {
        if (ImGui::MenuItem(LYRA_ICON_REFRESH " Refresh")) {
            // TODO: refresh
        }
        ImGui::Separator();
        if (ImGui::MenuItem(LYRA_ICON_NEW_FILE " New File")) {
            show_new_file_modal = true;
        }
        if (ImGui::MenuItem(LYRA_ICON_NEW_FOLDER " New Folder")) {
            show_new_folder_modal = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem(LYRA_ICON_IMPORT " Import")) {
            spdlog::info("Import Asset!");
        }
        ImGui::EndPopup();
    }

    // must be called outside of context menu
    if (show_new_file_modal) {
        ImGui::OpenPopup("New File");
    }

    // must be called outside of context menu
    if (show_new_folder_modal) {
        ImGui::OpenPopup("New Folder");
    }
}

void FileManager::show_new_file_dialog()
{
    if (ImGui::BeginPopupModal("New File", &show_new_file_modal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This is a modal dialog (file)!");
        ImGui::Separator();

        if (ImGui::Button("Close")) {
            show_new_file_modal = false;
            ImGui::CloseCurrentPopup(); // Close the current popup
        }

        ImGui::EndPopup();
    }
}

void FileManager::show_new_folder_dialog()
{
    if (ImGui::BeginPopupModal("New Folder", &show_new_folder_modal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This is a modal dialog (folder)!");
        ImGui::Separator();

        if (ImGui::Button("Close")) {
            show_new_folder_modal = false;
            ImGui::CloseCurrentPopup(); // Close the current popup
        }

        ImGui::EndPopup();
    }
}
