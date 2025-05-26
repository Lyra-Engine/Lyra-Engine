cmake_policy(VERSION 3.6)
include(FetchContent)

# define external project
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)

# get properties
FetchContent_GetProperties(glm)

# populate glm when needed
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
endif()

# add glm target (if not done so)
if(NOT TARGET glm)
  add_library(glm INTERFACE)
  target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
endif()

# mark glm as found
set(glm_FOUND TRUE)
