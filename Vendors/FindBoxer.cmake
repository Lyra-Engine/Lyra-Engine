include(FetchContent)

# define external project
FetchContent_Declare(
  boxer
  GIT_REPOSITORY https://github.com/aaronmjacobs/Boxer.git
  GIT_TAG        master
)

# get properties
FetchContent_GetProperties(boxer)

# build boxer when needed
set(BOXER_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(boxer)

# mark boxer as found
set(boxer_FOUND TRUE)

# Re-export target with namespace
add_library(boxer ALIAS Boxer)

# move Boxer under folder
set_target_properties(Boxer PROPERTIES FOLDER "Vendors")
