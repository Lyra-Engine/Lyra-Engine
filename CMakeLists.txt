cmake_minimum_required(VERSION 3.26)

# project name and version
project(LyraEngine VERSION 0.1 LANGUAGES C CXX)

# use only in-house cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Configs")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Vendors")

# settings
include(Compiler)
include(Project)
include(System)

# project library
add_subdirectory(Library)

# # project plugins
# add_subdirectory(Plugins/Window)
# add_subdirectory(Plugins/Vulkan)
# add_subdirectory(Plugins/Shader)

# # project samples
# add_subdirectory(Samples/Editor)
# add_subdirectory(Samples/Player)

add_executable(test ./Samples/test.cpp)
target_link_libraries(test PRIVATE lyra::common)
