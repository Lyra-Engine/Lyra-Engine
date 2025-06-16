include(FetchContent)

# define external project
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
)

# get properties
FetchContent_GetProperties(stb)

# populate stb
if(NOT imgui_POPULATED)
  FetchContent_Populate(stb)
endif()

# create an alias for the library (if not done so)
if(NOT TARGET stb)
  add_library(stb INTERFACE)
  target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
  set_target_properties(stb PROPERTIES FOLDER "Vendors")
endif()

# mark stb as found
set(stb_FOUND TRUE)
