#ifndef LYRA_PLUGIN_VULKAN_VKINCLUDE_H
#define LYRA_PLUGIN_VULKAN_VKINCLUDE_H

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef __linux__
#define VK_USE_PLATFORM_X11_KHR
#endif

#ifdef __APPLE__
#define VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_METAL_EXT
#endif

#include <volk.h>
#include <vk_mem_alloc.h>

#endif // LYRA_PLUGIN_VULKAN_VKINCLUDE_H
