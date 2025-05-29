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
)

# link dependencies
find_package(vma REQUIRED)
find_package(volk REQUIRED)
find_package(Vulkan REQUIRED)
target_link_libraries(lyra-vulkan PRIVATE volk Vulkan::Vulkan Vulkan::Headers VulkanMemoryAllocator)
target_compile_options(lyra-vulkan PRIVATE -Wno-nullability-completeness) # added for VulkanMemoryAllocator
