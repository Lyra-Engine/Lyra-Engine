include(FetchContent)

# define external project
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)

# get properties
FetchContent_GetProperties(glfw)

# build glfw when needed
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# mark glfw as found
set(glfw_FOUND TRUE)

# move to different folder
set_target_properties(glfw PROPERTIES FOLDER "Vendors")
set_target_properties(update_mappings PROPERTIES FOLDER "Vendors")
