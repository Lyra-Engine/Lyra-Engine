find_package(volk CONFIG)

if(NOT ${volk_FOUND})
  include(FetchContent)

  # define external project
  FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk
    GIT_TAG        1.4.304
  )

  # get properties
  FetchContent_GetProperties(volk)

  # build volk when needed
  FetchContent_MakeAvailable(volk)

  # mark volk as found
  set(volk_FOUND TRUE)

  # put volk under folder
  set_target_properties(volk PROPERTIES FOLDER "Vendors")
endif()
