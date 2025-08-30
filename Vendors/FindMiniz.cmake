find_package(miniz CONFIG)

if(NOT ${miniz_FOUND})
  include(FetchContent)

  # define external project
  FetchContent_Declare(
    miniz
    GIT_REPOSITORY https://github.com/richgel999/miniz.git
    GIT_TAG        3.0.2
  )

  # get properties
  FetchContent_GetProperties(miniz)

  # build miniz when needed
  set(INSTALL_PROJECT    OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTS        OFF CACHE BOOL "" FORCE)
  set(BUILD_EXAMPLES     OFF CACHE BOOL "" FORCE)
  set(BUILD_FUZZERS      OFF CACHE BOOL "" FORCE)
  set(AMALGAMATE_SOURCES OFF CACHE BOOL "" FORCE)
  set(BUILD_HEADER_ONLY  OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(miniz)

  # mark miniz as found
  set(miniz_FOUND TRUE)

  # move miniz under folder
  set_target_properties(miniz PROPERTIES FOLDER "Vendors")
endif()
