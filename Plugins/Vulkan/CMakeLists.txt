# setup plugin
lyra_plugin(vulkan)

# plugin sources
target_sources(lyra-vulkan PRIVATE
    Plugin.cpp
)

# link dependencies
find_package(vma REQUIRED)
find_package(volk REQUIRED)
find_package(Vulkan REQUIRED)
target_link_libraries(lyra-vulkan PRIVATE vma volk Vulkan::Vulkan)
