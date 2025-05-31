# setup plugin
lyra_plugin(vulkan)

# plugin sources
target_sources(lyra-vulkan PRIVATE
    Plugin.cpp
    VkUtils.h
    VkUtils.cpp
    VkMappings.cpp
    VkInstance.cpp
    VkSurface.cpp
    VkAdapter.cpp
    VkDevice.cpp
    VkSwapchain.cpp
    VkFence.cpp
    VkShader.cpp
    VkBuffer.cpp
    VkImage.cpp
    VkSampler.cpp
    VkLayout.cpp
)

# link dependencies
find_package(vma REQUIRED)
find_package(volk REQUIRED)
find_package(Vulkan REQUIRED)
target_link_libraries(lyra-vulkan PRIVATE volk Vulkan::Vulkan Vulkan::Headers VulkanMemoryAllocator)
target_compile_options(lyra-vulkan PRIVATE -Wno-nullability-completeness) # added for VulkanMemoryAllocator
