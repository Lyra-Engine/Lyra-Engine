#include <algorithm>
#include <Window/Window.hpp>
#include "VkUtils.h"

#ifdef USE_PLATFORM_WINDOWS
void add_surface_extension(Vector<const char*>& instance_extensions)
{
    instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_extensions.push_back("VK_KHR_win32_surface");
}

VkResult create_surface(VkInstance instance, const WindowHandle& window, VkSurfaceKHR& surface)
{
    static auto create = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");

    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd      = (HWND)window.native;
    createInfo.hinstance = GetModuleHandle(nullptr);
    if (create(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}
#endif

#ifdef USE_PLATFORM_LINUX
void add_surface_extension(Vector<const char*>& instance_extensions)
{
    instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_extensions.push_back("VK_KHR_xcb_surface");
}

VkResult create_surface(VkInstance instance, const WindowHandle& window, VkSurfaceKHR& surface)
{
    static auto create = (PFN_vkCreateXcbSurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateXcbSurfaceKHR");

    VkXcbSurfaceCreateInfoKHR createInfo{};
    createInfo.sType  = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.window = window.native;
    if (create(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}
#endif

#ifdef USE_PLATFORM_MACOS
void add_surface_extension(Vector<const char*>& instance_extensions)
{
    instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_extensions.push_back("VK_EXT_metal_surface");
    instance_extensions.push_back("VK_MVK_macos_surface");
}

VkResult create_surface(VkInstance instance, const WindowHandle& window, VkSurfaceKHR& surface)
{
    static auto create = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT");

    VkMetalSurfaceCreateInfoEXT metal_create_info{};
    metal_create_info.sType  = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    metal_create_info.pLayer = window.native;
    if (create(instance, &metal_create_info, nullptr, &surface) != VK_SUCCESS) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}
#endif

VkSurfaceKHR create_surface(VkInstance instance, const WindowHandle& handle)
{
    if (handle.window == nullptr)
        return VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    vk_check(create_surface(instance, handle, surface));
    return surface;
}

bool api::create_surface(GPUSurface& surface, const GPUSurfaceDescriptor& desc)
{
    auto rhi = get_rhi();

    rhi->surface_desc = desc;
    rhi->create_swapchain();
    return true;
}

void api::delete_surface()
{
    auto rhi = get_rhi();

    for (auto& swap_frame : rhi->swapchain_frames)
        swap_frame.destroy();
    rhi->swapchain_frames.clear();

    if (rhi->swapchain) {
        rhi->vtable.vkDestroySwapchainKHR(rhi->device, rhi->swapchain, nullptr);
        rhi->swapchain = VK_NULL_HANDLE;
    }

    if (rhi->surface) {
        vkDestroySurfaceKHR(rhi->instance, rhi->surface, nullptr);
        rhi->surface = VK_NULL_HANDLE;
    }
}
