find_package(magic_enum CONFIG)

if(NOT ${magic_enum_FOUND})
  include(FetchContent)

  # define external project
  FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        v0.9.7
  )

  # get properties
  FetchContent_GetProperties(magic_enum)

  # build magic_enum when needed
  set(MAGIC_ENUM_OPT_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
  set(MAGIC_ENUM_OPT_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
  set(MAGIC_ENUM_OPT_INSTALL         ON  CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(magic_enum)

  # mark magic_enum as found
  set(magic_enum_FOUND TRUE)

  # put magic_enum under folder
  set_target_properties(magic_enum PROPERTIES FOLDER "Vendors")
endif()
