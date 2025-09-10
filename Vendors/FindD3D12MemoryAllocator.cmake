include(FetchContent)

# define external project
FetchContent_Declare(
  D3D12MemoryAllocator
  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
  GIT_TAG        v3.0.1
)

# get properties
FetchContent_GetProperties(D3D12MemoryAllocator)

# build vma when needed
set(D3D12MA_BUILD_SAMPLE         OFF CACHE BOOL "" FORCE)
set(D3D12MA_BUILD_SAMPLE_SHADERS OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION          OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(D3D12MemoryAllocator)

# create an alias for the library (if not done so)
add_library(GPUOpen::D3D12MemoryAllocator ALIAS D3D12MemoryAllocator)
set_target_properties(D3D12MemoryAllocator PROPERTIES FOLDER "Vendors")

# mark vma as found
set(D3D12MemoryAllocator_FOUND TRUE)
