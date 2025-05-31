// windows native window
#ifdef USE_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

// macos native window
#ifdef USE_PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

// linux native window
#ifdef USE_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <Common/String.h>
#include <Common/Logger.h>
#include <Common/Plugin.h>
#include <Window/API.h>

using namespace lyra;
using namespace lyra::wsi;

static Logger logger = init_stderr_logger("GLFW", LogLevel::trace);

#ifdef USE_PLATFORM_WINDOWS
void populate_window_handle(GLFWwindow* win, WindowHandle& window)
{
    window.window = win;
    window.native = glfwGetWin32Window(win);
}
#endif

#ifdef USE_PLATFORM_MACOS
void* get_metal_layer(void* window);

void populate_window_handle(GLFWwindow* win, WindowHandle& window)
{
    window.window = win;
    window.native = get_metal_layer(glfwGetCocoaWindow(win));
}
#endif

#ifdef USE_PLATFORM_LINUX
void populate_window_handle(GLFWwindow* win, WindowHandle& window)
{
    window.window = win;
    window.native = glfwGetX11Window(win);
}
#endif

Logger get_logger()
{
    return logger;
}

auto get_api_name() -> CString
{
    return "Window";
}

void error_callback(int error, const char* description)
{
    get_logger()->error(description);
}

auto create_window(const WindowDescriptor& desc, WindowHandle& window) -> bool
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* win = glfwCreateWindow(desc.width, desc.height, desc.title, NULL, NULL);
    if (win != nullptr) {
        populate_window_handle(win, window);
    }
    return true;
}

void get_window_size(WindowHandle window, int& width, int& height)
{
    glfwGetWindowSize((GLFWwindow*)window.window, &width, &height);
}

auto delete_window(WindowHandle window) -> void
{
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(window.window));
}

void run_in_loop(WindowHandle window, WindowAPI::MainLoopCallback&& callback)
{
    auto handle = reinterpret_cast<GLFWwindow*>(window.window);

    // callbacks
    glfwSetWindowUserPointer(handle, &callback);
    glfwSetWindowSizeCallback(handle, [](GLFWwindow* window, int width, int height) {
        auto& cb = *static_cast<WindowAPI::MainLoopCallback*>(glfwGetWindowUserPointer(window));
        cb(WindowEvent::RESIZE);
    });

    // glfw main loop
    callback(WindowEvent::START);
    while (!glfwWindowShouldClose(handle)) {
        glfwPollEvents();

        callback(WindowEvent::UPDATE);
        callback(WindowEvent::RENDER);
    }
    callback(WindowEvent::CLOSE);
}

LYRA_EXPORT auto prepare() -> void
{
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);
}

LYRA_EXPORT auto cleanup() -> void
{
    glfwTerminate();
}

LYRA_EXPORT auto create() -> WindowAPI
{
    auto api            = WindowAPI{};
    api.get_api_name    = get_api_name;
    api.get_window_size = get_window_size;
    api.create_window   = create_window;
    api.delete_window   = delete_window;
    api.run_in_loop     = run_in_loop;
    return api;
}
