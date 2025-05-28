include(FetchContent)

# define external project
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)

# get properties
FetchContent_GetProperties(glm)

# add glm target (if not done so)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})

# mark glm as found
set(glm_FOUND TRUE)
