# enable natvis support
set(ENTT_INCLUDE_NATVIS ON)

find_package(EnTT CONFIG)

if(NOT ${EnTT_FOUND})
  include(FetchContent)

  # define external project
  FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.13.2
  )

  # get properties
  FetchContent_GetProperties(entt)

  # populate entt when needed
  if(NOT entt_POPULATED)
      FetchContent_Populate(entt)
  endif()

  # add stb target (if not done so)
  if(NOT TARGET entt)
    add_library(entt INTERFACE)
    add_library(EnTT::EnTT ALIAS entt)
    target_include_directories(entt INTERFACE ${entt_SOURCE_DIR}/single_include)
  endif()

  # mark stb as found
  set(entt_FOUND TRUE)

  # put stb under folder
  set_target_properties(entt PROPERTIES FOLDER "Vendors")
endif()
