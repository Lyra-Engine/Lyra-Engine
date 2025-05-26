include(FetchContent)

# define external project
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG        v3.1.1
)

# get properties
FetchContent_GetProperties(cxxopts)

# build cxxopts when needed
set(CXXOPTS_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_INSTALL  OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_WARNINGS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cxxopts)

# mark cxxopts as found
set(cxxopts_FOUND TRUE)
