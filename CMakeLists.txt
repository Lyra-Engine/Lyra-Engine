cmake_minimum_required(VERSION 2.8...3.26)

# project name and version
project(Lyra-Engine VERSION 0.1 LANGUAGES C CXX)

# use only in-house cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Configs")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/Vendors")

# settings
include(Compiler)
include(Project)
include(System)

# project components
add_subdirectory(Library)
add_subdirectory(Plugins)
add_subdirectory(Samples)
add_subdirectory(TestLib)
