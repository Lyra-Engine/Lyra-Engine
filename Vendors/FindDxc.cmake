cmake_policy(VERSION 3.6)
include(FetchContent)

# define external project
FetchContent_Declare(
  dxc
  GIT_REPOSITORY https://github.com/microsoft/DirectXShaderCompiler.git
  GIT_TAG        v1.8.2405
)

# get properties
FetchContent_GetProperties(dxc)

# populate dxc when needed
if(NOT dxc_POPULATED)
    FetchContent_Populate(dxc)
endif()

# add dxc target (if not done so)
if(NOT TARGET dxc)
  include(${dxc_SOURCE_DIR}/cmake/caches/PredefinedParams.cmake)
  FetchContent_MakeAvailable(dxc)
endif()

# mark dxc as found
set(dxc_FOUND TRUE)
