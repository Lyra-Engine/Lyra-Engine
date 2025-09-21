#include <Lyra/AppKit/Builtin/LayoutManager.h>

using namespace lyra;

LayoutManager::LayoutManager(const LayoutDescriptor& descriptor) : descriptor(descriptor)
{
    // do nothing
}

void LayoutManager::bind(Application& app)
{
    // bind layout manager events
    app.bind<AppEvent::UPDATE>(&LayoutManager::update, this);
}

void LayoutManager::update(Blackboard& blackboard)
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

LayoutInfo LayoutManager::init() const
{
    switch (descriptor.mode) {
        case LayoutMode::EDITOR:
            return create_editor_layout(descriptor);
        default:
            return create_player_layout();
    }
}

LayoutInfo LayoutManager::create_player_layout() const
{
    LayoutInfo layout = {};
    layout.main       = ImGui::GetMainViewport()->ID;
    return layout;
}

LayoutInfo LayoutManager::create_editor_layout(const LayoutDescriptor& desc) const
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
