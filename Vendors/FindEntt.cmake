# NOTE: Don't use vcpkg here, because entt needs to be included in installation.

# enable natvis support
set(ENTT_INCLUDE_NATVIS ON)

include(FetchContent)

# define external project
FetchContent_Declare(
  entt
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.13.2
)

# get properties
FetchContent_GetProperties(entt)

# build entt when needed (enable install)
set(ENTT_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENTT_INSTALL       ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(entt)

# mark stb as found
set(entt_FOUND TRUE)

# put stb under folder
set_target_properties(EnTT PROPERTIES FOLDER "Vendors")
