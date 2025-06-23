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
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp")

# install shared library libraries
install(
    DIRECTORY $<TARGET_FILE_DIR:lyra-engine>/
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/$<CONFIG>/
    FILES_MATCHING
        PATTERN "*.dll"
        PATTERN "*.lib")

# install the target file
install(
    EXPORT lyra-engine-targets
    FILE lyra-engine-targets.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/Lyra-Engine)

# configure package
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/Targets/LyraEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/Lyra-EngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/Lyra-Engine)

# install package config
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/Lyra-EngineConfig.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/Lyra-Engine)
