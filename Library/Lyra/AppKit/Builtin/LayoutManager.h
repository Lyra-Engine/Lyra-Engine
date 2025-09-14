#pragma once

#ifndef LYRA_LIBRARY_APPKIT_LAYOUT_MANAGER_H
#define LYRA_LIBRARY_APPKIT_LAYOUT_MANAGER_H

#include <imgui.h>
#include <imgui_internal.h>
#include <Lyra/AppKit/AppTypes.h>

namespace lyra
{
    enum struct LayoutMode
    {
        EDITOR,
        PLAYER,
    };

    // ImGuiID == 0 means this panel does not exist
    struct LayoutInfo
    {
        ImGuiID main   = 0;
        ImGuiID left   = 0;
        ImGuiID right  = 0;
        ImGuiID top    = 0;
        ImGuiID bottom = 0;
    };

    struct LayoutDescriptor
    {
        LayoutMode mode   = LayoutMode::EDITOR;
        float      left   = 0.25f;
        float      right  = 0.25f;
        float      top    = 0.25f;
        float      bottom = 0.25f;
    };

    // player mode does not need any docking layout
    inline LayoutInfo create_player_layout()
    {
        LayoutInfo layout = {};
        layout.main       = ImGui::GetMainViewport()->ID;
        return layout;
    }

    // editor mode has support for docking on every direction
    inline LayoutInfo create_editor_layout(const LayoutDescriptor& desc)
    {
        ImGuiID dockspace_id = ImGui::GetMainViewport()->ID;

        // clear old layout
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        // split the dockspace into regions
        LayoutInfo layout = {};
        layout.main       = dockspace_id;
        layout.left       = ImGui::DockBuilderSplitNode(layout.main, ImGuiDir_Left, desc.left, nullptr, &layout.main);
        layout.right      = ImGui::DockBuilderSplitNode(layout.main, ImGuiDir_Right, desc.right, nullptr, &layout.main);
        layout.top        = ImGui::DockBuilderSplitNode(layout.main, ImGuiDir_Up, desc.top, nullptr, &layout.main);
        layout.bottom     = ImGui::DockBuilderSplitNode(layout.main, ImGuiDir_Down, desc.bottom, nullptr, &layout.main);

        // finish
        ImGui::DockBuilderFinish(dockspace_id);
        return layout;
    }

    struct LayoutManager
    {
    public:
        explicit LayoutManager(const LayoutDescriptor& descriptor) : descriptor(descriptor) {}

        void bind(Application& app)
        {
            // bind layout manager events
            app.bind<AppEvent::UPDATE>(&LayoutManager::update, this);
        }

        void update(Blackboard& blackboard)
        {
            ImGuiID dockspace_id = ImGui::GetMainViewport()->ID;
            ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());

            // running dock builder exactly once
            static bool first_launch = true;
            if (first_launch) {
                first_launch = false;
                blackboard.add<LayoutInfo>(init());
            }
        }

    private:
        auto init() const -> LayoutInfo
        {
            switch (descriptor.mode) {
                case LayoutMode::EDITOR:
                    return create_editor_layout(descriptor);
                default:
                    return create_player_layout();
            }
        }

    private:
        LayoutDescriptor descriptor = {};
    };

} // namespace lyra

#endif // LYRA_LIBRARY_APPKIT_LAYOUT_MANAGER_H
