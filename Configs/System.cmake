# vendor path
set(PROJECT_VENDOR_PATH "${PROJECT_SOURCE_DIR}/Vendors")

# detect architecture
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
  message(STATUS "Target architecture: x64")
  set(PROJECT_ARCH "x64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i686" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
  message(STATUS "Target architecture: x86")
  set(PROJECT_ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  message(STATUS "Target architecture: ARM64")
  set(PROJECT_ARCH "arm64")
else()
  message(FATAL_ERROR "Target architecture: unknown")
endif()

# detect operating system
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message(STATUS "Target OS: Windows")
  add_compile_definitions(USE_PLATFORM_WINDOWS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(STATUS "Target OS: Linux")
  add_compile_definitions(USE_PLATFORM_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  message(STATUS "Target OS: MacOs")
  add_compile_definitions(USE_PLATFORM_MACOS)
else()
  message(FATAL_ERROR "Target OS: Unknown")
endif()
