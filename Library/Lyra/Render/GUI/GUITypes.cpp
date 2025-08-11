#include <Lyra/Render/GUI/GUITypes.h>

using namespace lyra;
using namespace lyra::gui;

using GUIRenderPlugin = Plugin<GUIRenderAPI>;

Own<GUIRenderPlugin> GUI_RENDER_PLUGIN = nullptr;

#pragma region GUI
GUIRenderAPI* GUI::api()
{
    return GUI_RENDER_PLUGIN->get_api();
}

void GUI::init_plugin()
{
    if (!GUI_RENDER_PLUGIN)
        GUI_RENDER_PLUGIN = std::make_unique<GUIRenderPlugin>("lyra-imgui");
}
#pragma endregion GUI
