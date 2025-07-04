find_package(VulkanMemoryAllocator CONFIG)

if(NOT ${VulkanMemoryAllocator_FOUND})
  include(FetchContent)

  # define external project
  FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG        v3.3.0
  )

  # get properties
  FetchContent_GetProperties(VulkanMemoryAllocator)

  # build vma when needed
  set(VMA_BUILD_SAMPLE         OFF CACHE BOOL "" FORCE)
  set(VMA_BUILD_SAMPLE_SHADERS OFF CACHE BOOL "" FORCE)
  set(BUILD_DOCUMENTATION      OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(VulkanMemoryAllocator)

  # create an alias for the library (if not done so)
  if(NOT TARGET VulkanMemoryAllocator)
    add_library(VulkanMemoryAllocator ALIAS GPUOpen::VulkanMemoryAllocator)
    set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER "Vendors")
  endif()

  # mark vma as found
  set(VulkanMemoryAllocator_FOUND TRUE)
endif()
