# setup plugin
lyra_plugin(imgui)

# plugin sources
target_sources(lyra-imgui PRIVATE
    ImGuiPlugin.cpp
    GuiRenderer.h
    GuiRenderer.cpp
)
