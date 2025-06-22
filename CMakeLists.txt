cmake_minimum_required(VERSION 3.6...3.27)

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

# install shared library header
install(
    DIRECTORY Library/
    DESTINATION include/
    FILES_MATCHING PATTERN "*.[h|hpp]")

# install the target file
install(
    EXPORT lyra-engine-targets
    FILE lyra-engine-targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Lyra)

# configure package
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/Targets/LyraEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/LyraEngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Lyra
)

# install package config
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/LyraEngineConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Lyra
)
