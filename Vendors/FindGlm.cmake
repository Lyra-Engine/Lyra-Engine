include(FetchContent)

# define external project
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.1
)

# get properties
FetchContent_GetProperties(glm)

# build glm when needed (enable install)
set(GLM_BUILD_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

# mark glm as found
set(glm_FOUND TRUE)

# put glm under folder
set_target_properties(glm PROPERTIES FOLDER "Vendors")
