include(FetchContent)

# define external project
FetchContent_Declare(
  tinygltf
  GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
  GIT_TAG        v2.8.18
)

# get properties
FetchContent_GetProperties(tinygltf)

# build tinygltf when needed
set(TINYGLTF_BUILD_LOADER_EXAMPLE    OFF CACHE BOOL "" FORCE)
set(TINYGLTF_BUILD_GL_EXAMPLE        OFF CACHE BOOL "" FORCE)
set(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF CACHE BOOL "" FORCE)
set(TINYGLTF_BUILD_BUILDER_EXAMPLE   OFF CACHE BOOL "" FORCE)
set(TINYGLTF_HEADER_ONLY             OFF CACHE BOOL "" FORCE)
set(TINYGLTF_INSTALL                 OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(tinygltf)

# add additional include path
include_directories("${tinygltf_SOURCE_DIR}/src")

# mark tinygltf as found
set(tinygltf_FOUND TRUE)
