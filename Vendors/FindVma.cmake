include(FetchContent)

# define external project
FetchContent_Declare(
  vma
  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
  GIT_TAG        v3.0.1
)

# get properties
FetchContent_GetProperties(vma)

# build vma when needed
set(VMA_BUILD_SAMPLE         OFF CACHE BOOL "" FORCE)
set(VMA_BUILD_SAMPLE_SHADERS OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION      OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(vma)

# create an alias for the library (if not done so)
if(NOT TARGET vma)
  add_library(vma ALIAS VulkanMemoryAllocator)
  set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER "Vendors")
endif()

# mark vma as found
set(vma_FOUND TRUE)
