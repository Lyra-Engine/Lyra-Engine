// windows native window
#include <algorithm>
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

#include <Lyra/Common/String.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Plugin.h>
#include <Lyra/Window/WSIAPI.h>
#include <Lyra/Window/WSITypes.h>
#include <Lyra/Render/RHI/Types.h>

using namespace lyra;
using namespace lyra::rhi;
using namespace lyra::wsi;

static Logger logger = init_stderr_logger("GLFW", LogLevel::trace);

struct UserState
{
    WindowInputState input_state;
    WindowCallback   callback;
};

struct EventLoopInternal
{
    Vector<WindowHandle> windows;

    bool should_exit() const { return windows.empty(); }
};

static EventLoopInternal global_event_loop;

#ifdef USE_PLATFORM_WINDOWS
void create_window_handle(GLFWwindow* win, WindowHandle& window)
{
    window.window = win;
    window.native = glfwGetWin32Window(win);
}
#endif

#ifdef USE_PLATFORM_MACOS
void* get_metal_layer(void* window);

void create_window_handle(GLFWwindow* win, WindowHandle& window)
{
    window.window = win;
    window.native = get_metal_layer(glfwGetCocoaWindow(win));
}
#endif

#ifdef USE_PLATFORM_LINUX
void create_window_handle(GLFWwindow* win, WindowHandle& window)
{
    window.window = win;
    window.native = glfwGetX11Window(win);
}
#endif

Logger get_logger()
{
    return logger;
}

ButtonState to_button_state(int action)
{
    switch (action) {
        case GLFW_PRESS:
            return ButtonState::ON;
        case GLFW_RELEASE:
            return ButtonState::OFF;
        case GLFW_REPEAT:
            return ButtonState::ON;
    }
    throw std::invalid_argument("Unsupport GLFW keyboard action!");
}

void update_mods(WindowInputState& state, int mods)
{
    if (mods & GLFW_MOD_ALT)
        state.keyboard.modifiers.set(Modifier::ALT);
    else
        state.keyboard.modifiers.unset(Modifier::ALT);

    if (mods & GLFW_MOD_SHIFT)
        state.keyboard.modifiers.set(Modifier::SHIFT);
    else
        state.keyboard.modifiers.unset(Modifier::SHIFT);

    if (mods & GLFW_MOD_CONTROL)
        state.keyboard.modifiers.set(Modifier::CTRL);
    else
        state.keyboard.modifiers.unset(Modifier::CTRL);
}

auto get_api_name() -> CString
{
    return "Window";
}

void error_callback(int error, const char* description)
{
    get_logger()->error(description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto& user  = *static_cast<UserState*>(glfwGetWindowUserPointer(window));
    auto& state = user.input_state;
    update_mods(state, mods);

    // clang-format off
    switch (key) {
        case GLFW_KEY_TAB:       state.keyboard.status.at((int)KeyButton::TAB)       = to_button_state(action); break;
        case GLFW_KEY_ESCAPE:    state.keyboard.status.at((int)KeyButton::ESC)       = to_button_state(action); break;
        case GLFW_KEY_SPACE:     state.keyboard.status.at((int)KeyButton::SPACE)     = to_button_state(action); break;
        case GLFW_KEY_BACKSPACE: state.keyboard.status.at((int)KeyButton::BACKSPACE) = to_button_state(action); break;
        case GLFW_KEY_DELETE:    state.keyboard.status.at((int)KeyButton::DEL)       = to_button_state(action); break;
        case GLFW_KEY_ENTER:     state.keyboard.status.at((int)KeyButton::ENTER)     = to_button_state(action); break;
        case GLFW_KEY_PAGE_UP:   state.keyboard.status.at((int)KeyButton::PAGE_UP)   = to_button_state(action); break;
        case GLFW_KEY_PAGE_DOWN: state.keyboard.status.at((int)KeyButton::PAGE_DOWN) = to_button_state(action); break;
        case GLFW_KEY_HOME:      state.keyboard.status.at((int)KeyButton::HOME)      = to_button_state(action); break;
        case GLFW_KEY_END:       state.keyboard.status.at((int)KeyButton::END)       = to_button_state(action); break;

        case GLFW_KEY_UP:        state.keyboard.status.at((int)KeyButton::UP)        = to_button_state(action); break;
        case GLFW_KEY_DOWN:      state.keyboard.status.at((int)KeyButton::DOWN)      = to_button_state(action); break;
        case GLFW_KEY_LEFT:      state.keyboard.status.at((int)KeyButton::LEFT)      = to_button_state(action); break;
        case GLFW_KEY_RIGHT:     state.keyboard.status.at((int)KeyButton::RIGHT)     = to_button_state(action); break;

        case GLFW_KEY_0:         state.keyboard.status.at((int)KeyButton::D0)        = to_button_state(action); break;
        case GLFW_KEY_1:         state.keyboard.status.at((int)KeyButton::D1)        = to_button_state(action); break;
        case GLFW_KEY_2:         state.keyboard.status.at((int)KeyButton::D2)        = to_button_state(action); break;
        case GLFW_KEY_3:         state.keyboard.status.at((int)KeyButton::D3)        = to_button_state(action); break;
        case GLFW_KEY_4:         state.keyboard.status.at((int)KeyButton::D4)        = to_button_state(action); break;
        case GLFW_KEY_5:         state.keyboard.status.at((int)KeyButton::D5)        = to_button_state(action); break;
        case GLFW_KEY_6:         state.keyboard.status.at((int)KeyButton::D6)        = to_button_state(action); break;
        case GLFW_KEY_7:         state.keyboard.status.at((int)KeyButton::D7)        = to_button_state(action); break;
        case GLFW_KEY_8:         state.keyboard.status.at((int)KeyButton::D8)        = to_button_state(action); break;
        case GLFW_KEY_9:         state.keyboard.status.at((int)KeyButton::D9)        = to_button_state(action); break;

        case GLFW_KEY_F1:        state.keyboard.status.at((int)KeyButton::F1)        = to_button_state(action); break;
        case GLFW_KEY_F2:        state.keyboard.status.at((int)KeyButton::F2)        = to_button_state(action); break;
        case GLFW_KEY_F3:        state.keyboard.status.at((int)KeyButton::F3)        = to_button_state(action); break;
        case GLFW_KEY_F4:        state.keyboard.status.at((int)KeyButton::F4)        = to_button_state(action); break;
        case GLFW_KEY_F5:        state.keyboard.status.at((int)KeyButton::F5)        = to_button_state(action); break;
        case GLFW_KEY_F6:        state.keyboard.status.at((int)KeyButton::F6)        = to_button_state(action); break;
        case GLFW_KEY_F7:        state.keyboard.status.at((int)KeyButton::F7)        = to_button_state(action); break;
        case GLFW_KEY_F8:        state.keyboard.status.at((int)KeyButton::F8)        = to_button_state(action); break;
        case GLFW_KEY_F9:        state.keyboard.status.at((int)KeyButton::F9)        = to_button_state(action); break;
        case GLFW_KEY_F10:       state.keyboard.status.at((int)KeyButton::F10)       = to_button_state(action); break;
        case GLFW_KEY_F11:       state.keyboard.status.at((int)KeyButton::F11)       = to_button_state(action); break;
        case GLFW_KEY_F12:       state.keyboard.status.at((int)KeyButton::F12)       = to_button_state(action); break;

        case GLFW_KEY_A:         state.keyboard.status.at((int)KeyButton::A)         = to_button_state(action); break;
        case GLFW_KEY_B:         state.keyboard.status.at((int)KeyButton::B)         = to_button_state(action); break;
        case GLFW_KEY_C:         state.keyboard.status.at((int)KeyButton::C)         = to_button_state(action); break;
        case GLFW_KEY_D:         state.keyboard.status.at((int)KeyButton::D)         = to_button_state(action); break;
        case GLFW_KEY_E:         state.keyboard.status.at((int)KeyButton::E)         = to_button_state(action); break;
        case GLFW_KEY_F:         state.keyboard.status.at((int)KeyButton::F)         = to_button_state(action); break;
        case GLFW_KEY_G:         state.keyboard.status.at((int)KeyButton::G)         = to_button_state(action); break;
        case GLFW_KEY_H:         state.keyboard.status.at((int)KeyButton::H)         = to_button_state(action); break;
        case GLFW_KEY_I:         state.keyboard.status.at((int)KeyButton::I)         = to_button_state(action); break;
        case GLFW_KEY_J:         state.keyboard.status.at((int)KeyButton::J)         = to_button_state(action); break;
        case GLFW_KEY_K:         state.keyboard.status.at((int)KeyButton::K)         = to_button_state(action); break;
        case GLFW_KEY_L:         state.keyboard.status.at((int)KeyButton::L)         = to_button_state(action); break;
        case GLFW_KEY_M:         state.keyboard.status.at((int)KeyButton::M)         = to_button_state(action); break;
        case GLFW_KEY_N:         state.keyboard.status.at((int)KeyButton::N)         = to_button_state(action); break;
        case GLFW_KEY_O:         state.keyboard.status.at((int)KeyButton::O)         = to_button_state(action); break;
        case GLFW_KEY_P:         state.keyboard.status.at((int)KeyButton::P)         = to_button_state(action); break;
        case GLFW_KEY_Q:         state.keyboard.status.at((int)KeyButton::Q)         = to_button_state(action); break;
        case GLFW_KEY_R:         state.keyboard.status.at((int)KeyButton::R)         = to_button_state(action); break;
        case GLFW_KEY_S:         state.keyboard.status.at((int)KeyButton::S)         = to_button_state(action); break;
        case GLFW_KEY_T:         state.keyboard.status.at((int)KeyButton::T)         = to_button_state(action); break;
        case GLFW_KEY_U:         state.keyboard.status.at((int)KeyButton::U)         = to_button_state(action); break;
        case GLFW_KEY_V:         state.keyboard.status.at((int)KeyButton::V)         = to_button_state(action); break;
        case GLFW_KEY_W:         state.keyboard.status.at((int)KeyButton::W)         = to_button_state(action); break;
        case GLFW_KEY_X:         state.keyboard.status.at((int)KeyButton::X)         = to_button_state(action); break;
        case GLFW_KEY_Y:         state.keyboard.status.at((int)KeyButton::Y)         = to_button_state(action); break;
        case GLFW_KEY_Z:         state.keyboard.status.at((int)KeyButton::Z)         = to_button_state(action); break;

        // ignore control keys
        case GLFW_KEY_CAPS_LOCK:
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_LEFT_SUPER:
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_ALT:
        case GLFW_KEY_RIGHT_SHIFT:
        case GLFW_KEY_RIGHT_SUPER:
        case GLFW_KEY_RIGHT_CONTROL:
            break;

        default: get_logger()->error("[GLFW] unhandled key stroke!");
    }
    // clang-format off
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto& user  = *static_cast<UserState*>(glfwGetWindowUserPointer(window));
    auto& state = user.input_state;
    update_mods(state, mods);

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            state.mouse.status[int(MouseButton::LEFT)] = to_button_state(action);
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            state.mouse.status[int(MouseButton::MIDDLE)] = to_button_state(action);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            state.mouse.status[int(MouseButton::RIGHT)] = to_button_state(action);
            break;
        default:
            break;
    }
}

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto& user  = *static_cast<UserState*>(glfwGetWindowUserPointer(window));
    auto& state = user.input_state;
    state.mouse.position.x = static_cast<float>(xpos);
    state.mouse.position.y = static_cast<float>(ypos);
}

void window_close_callback(GLFWwindow* window) {
    // clean up user states
    auto user = static_cast<UserState*>(glfwGetWindowUserPointer(window));
    user->callback(WindowEvent::CLOSE);
    delete user;

    // find corresponding window handle
    auto it = std::find_if(
            global_event_loop.windows.begin(),
            global_event_loop.windows.end(),
            [&](const WindowHandle& handle) {
            return window == reinterpret_cast<GLFWwindow*>(handle.window);
            });

    // remove window from tracking
    if (it != global_event_loop.windows.end())
        global_event_loop.windows.erase(it);
}

void bind_window_events(WindowHandle window)
{
    auto user        = new UserState{};
    user->input_state = {};

    // callback
    auto handle = reinterpret_cast<GLFWwindow*>(window.window);
    glfwSetWindowUserPointer(handle, user);
    glfwSetWindowCloseCallback(handle, window_close_callback);
    glfwSetKeyCallback(handle, key_callback);
    glfwSetCursorPosCallback(handle, mouse_position_callback);
    glfwSetMouseButtonCallback(handle, mouse_button_callback);
    glfwSetWindowSizeCallback(handle, [](GLFWwindow* window, int width, int height) {
        auto& user = *static_cast<UserState*>(glfwGetWindowUserPointer(window));
        std::invoke(user.callback, WindowEvent::RESIZE);
    });
}

auto create_window(const WindowDescriptor& desc, WindowHandle& window) -> bool
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* win = glfwCreateWindow(desc.width, desc.height, desc.title, NULL, NULL);
    if (win != nullptr) {
        create_window_handle(win, window);
        bind_window_events(window);
        global_event_loop.windows.push_back(window);
    }
    return true;
}

bool get_window_size(WindowHandle window, uint& width, uint& height)
{
    int32_t w, h;
    glfwGetWindowSize((GLFWwindow*)window.window, &w, &h);
    width  = static_cast<uint32_t>(w);
    height = static_cast<uint32_t>(h);
    return true;
}

bool get_input_state(WindowHandle window, WindowInputState& state)
{
    auto  handle = reinterpret_cast<GLFWwindow*>(window.window);
    auto& user   = *static_cast<UserState*>(glfwGetWindowUserPointer(handle));

    // copy the input state back to user
    state = user.input_state;
    return true;
}

auto delete_window(WindowHandle window) -> void
{
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(window.window));
}

bool bind_window_callback(WindowHandle window, WindowCallback&& callback)
{
    auto  handle = reinterpret_cast<GLFWwindow*>(window.window);
    auto& user   = *static_cast<UserState*>(glfwGetWindowUserPointer(handle));
    user.callback = std::move(callback);
    return true;
}

void run_in_loop()
{
    // START
    for (auto& window : global_event_loop.windows) {
        auto handle = reinterpret_cast<GLFWwindow*>(window.window);
        auto& user   = *static_cast<UserState*>(glfwGetWindowUserPointer(handle));
        std::invoke(user.callback, WindowEvent::START);
    }

    // glfw main loop
    while (!global_event_loop.should_exit()) {

        // UPDATE
        for (auto& window : global_event_loop.windows) {
            auto handle = reinterpret_cast<GLFWwindow*>(window.window);
            auto& user   = *static_cast<UserState*>(glfwGetWindowUserPointer(handle));
            std::invoke(user.callback, WindowEvent::UPDATE);
        }

        // RENDER
        RHI::new_frame(); // prior to frame begin
        for (auto& window : global_event_loop.windows) {
            auto handle = reinterpret_cast<GLFWwindow*>(window.window);
            auto& user   = *static_cast<UserState*>(glfwGetWindowUserPointer(handle));
            std::invoke(user.callback, WindowEvent::RENDER);
        }
        RHI::end_frame(); // post frame end

        glfwPollEvents();
    }

    // wait until GPU device is idle
    RHI::wait();
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
    auto api                 = WindowAPI{};
    api.get_api_name         = get_api_name;
    api.get_window_size      = get_window_size;
    api.get_input_state      = get_input_state;
    api.create_window        = create_window;
    api.delete_window        = delete_window;
    api.bind_window_callback = bind_window_callback;
    api.run_in_loop          = run_in_loop;
    return api;
}
