include(FetchContent)

# define external project
# use a newer commit version to avoid cmake deprecation warning
FetchContent_Declare(
  ktx
  GIT_REPOSITORY https://github.com/richgel999/ktx.git
  GIT_TAG        c883286 # 3.0.2
)

# get properties
FetchContent_GetProperties(ktx)

# build ktx when needed
set(INSTALL_PROJECT    OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES     OFF CACHE BOOL "" FORCE)
set(BUILD_FUZZERS      OFF CACHE BOOL "" FORCE)
set(AMALGAMATE_SOURCES OFF CACHE BOOL "" FORCE)
set(BUILD_HEADER_ONLY  OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ktx)

# mark ktx as found
set(ktx_FOUND TRUE)

# move ktx under folder
set_target_properties(ktx PROPERTIES FOLDER "Vendors")
