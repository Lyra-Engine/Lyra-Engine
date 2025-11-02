#pragma once

#ifndef LYRA_LIBRARY_EDITOR_LAYOUT_MANAGER_H
#define LYRA_LIBRARY_EDITOR_LAYOUT_MANAGER_H

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

    // LayoutDescriptor is used for configuring docking splits.
    struct LayoutDescriptor
    {
        LayoutMode mode   = LayoutMode::EDITOR;
        float      left   = 0.25f;
        float      right  = 0.25f;
        float      top    = 0.25f;
        float      bottom = 0.25f;
    };

    // LayoutManager is only a wrapper for ImGui's docking builder.
    // It is created to fit AppKit's style of data and event hanlding.
    struct LayoutManager
    {
    public:
        explicit LayoutManager(const LayoutDescriptor& descriptor);

        void bind(Application& app);

        void update(Blackboard& blackboard);

    private:
        LayoutInfo init() const;

        // player mode does not need any docking layout
        LayoutInfo create_player_layout() const;

        // editor mode has support for docking on every direction
        LayoutInfo create_editor_layout(const LayoutDescriptor& desc) const;

    private:
        LayoutDescriptor descriptor = {};
    };

} // namespace lyra

#endif // LYRA_LIBRARY_EDITOR_LAYOUT_MANAGER_H
