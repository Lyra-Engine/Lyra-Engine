include(FetchContent)

# define external project
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.3
)

# get properties
FetchContent_GetProperties(spdlog)

# build spdlog when needed
set(SPDLOG_BUILD_ALL     OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED  OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# mark spdlog as found
set(spdlog_FOUND TRUE)

set_target_properties(spdlog PROPERTIES FOLDER "Vendors")
