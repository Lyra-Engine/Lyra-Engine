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

    // TODO: create swapchain
    SwapchainSupportDetails swapchain_support = query_swapchain_support(rhi->adapter, rhi->surface);
    VkSurfaceFormatKHR      surface_format    = choose_swap_surface_format(swapchain_support.formats);
    VkPresentModeKHR        present_mode      = choose_swap_present_mode(swapchain_support.present_modes);
    VkExtent2D              extent            = choose_swap_extent(desc, swapchain_support.capabilities);

    uint32_t image_count = std::clamp(desc.frames_inflight, swapchain_support.capabilities.minImageCount, swapchain_support.capabilities.maxImageCount);

    auto create_info             = VkSwapchainCreateInfoKHR{};
    create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface          = rhi->surface;
    create_info.minImageCount    = image_count;
    create_info.imageFormat      = surface_format.format;
    create_info.imageColorSpace  = surface_format.colorSpace;
    create_info.imageExtent      = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    create_info.clipped          = VK_TRUE;
    create_info.presentMode      = present_mode;
    create_info.preTransform     = swapchain_support.capabilities.currentTransform;
    create_info.compositeAlpha   = vkenum(desc.alpha_mode);

    auto indices              = find_queue_family_indices(rhi->adapter, rhi->surface);
    uint queueFamilyIndices[] = {indices.graphics.value(), indices.present.value()};
    if (indices.graphics != indices.present) {
        create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queueFamilyIndices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (rhi->swapchain) {
        create_info.oldSwapchain = rhi->swapchain;
    }

    rhi->swapchain_colorspace = surface_format.colorSpace;
    rhi->swapchain_format     = surface_format.format;
    rhi->swapchain_dim        = extent;
    vk_check(rhi->vtable.vkCreateSwapchainKHR(rhi->device, &create_info, nullptr, &rhi->swapchain));

    // get swapchain images
    uint count;
    vk_check(rhi->vtable.vkGetSwapchainImagesKHR(rhi->device, rhi->swapchain, &count, nullptr));

    Vector<VkImage> swapchain_images(count);
    vk_check(rhi->vtable.vkGetSwapchainImagesKHR(rhi->device, rhi->swapchain, &count, swapchain_images.data()));

    // create swapchain data
    if (rhi->swapchain_frames.empty()) {
        rhi->swapchain_frames.resize(count);
        for (uint i = 0; i < count; i++)
            rhi->swapchain_frames.at(i).init(swapchain_images.at(i), surface_format.format, extent);
    }

    // create logical frames in flight
    if (rhi->frames.empty()) {
        rhi->frames.resize(desc.frames_inflight);
        for (auto& frame : rhi->frames)
            frame.init();
    }

    // reset
    rhi->current_frame_index = 0;
    rhi->current_image_index = 0;
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
