# setup plugin
lyra_plugin(glfw)

# plugin sources
target_sources(lyra-glfw PRIVATE Plugin.cpp)

# link dependencies
find_package(glfw REQUIRED)
target_link_libraries(lyra-glfw PRIVATE glfw)
