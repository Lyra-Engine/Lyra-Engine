find_package(physfs CONFIG)

if(NOT ${physfs_FOUND})
  include(FetchContent)

  # define external project
  # use a newer commit version to avoid cmake deprecation warning
  FetchContent_Declare(
    physfs
    GIT_REPOSITORY https://github.com/icculus/physfs.git
    GIT_TAG        9d18d36 # release-3.2.0
  )

  # get properties
  FetchContent_GetProperties(physfs)

  # build physfs when needed
  set(PHYSFS_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
  set(PHYSFS_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
  set(PHYSFS_BUILD_TEST     OFF CACHE BOOL "" FORCE)
  set(PHYSFS_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(physfs)

  # mark physfs as found
  set(physfs_FOUND TRUE)

  # move physfs under folder
  set_target_properties(physfs-static PROPERTIES FOLDER "Vendors")
endif()
