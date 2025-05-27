#include <Common/Plugin.h>
#include <Window/Types.h>
#include <Window/API.h>

using namespace lyra;
using namespace lyra::wsi;

using WindowPlugin = Plugin<WindowAPI>;

static Own<WindowPlugin> WINDOW_PLUGIN;

Window Window::init(const WindowDescriptor& descriptor)
{
    if (!WINDOW_PLUGIN)
        WINDOW_PLUGIN = std::make_unique<WindowPlugin>("lyra-glfw");

    Window window;
    WINDOW_PLUGIN->get_api()->create_window(descriptor, window.handle);
    return window;
}

void Window::destroy()
{
    WINDOW_PLUGIN->get_api()->delete_window(this->handle);
}

void Window::loop()
{
    WINDOW_PLUGIN->get_api()->run_in_loop(handle, [&](WindowEvent event) {
        size_t index = static_cast<size_t>(event);
        callbacks.at(index)();
    });
}
