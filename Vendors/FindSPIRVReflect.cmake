include(FetchContent)

# define external project
FetchContent_Declare(
  spirvreflect
  GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
  GIT_TAG        vulkan-sdk-1.3.280.0
)

# get properties
FetchContent_GetProperties(spirvreflect)

# populate SPIRVReflect
if(NOT spirvreflect_POPULATED)
  FetchContent_Populate(spirvreflect)
endif()

# configure library (if not done so)
if(NOT TARGET spirvreflect)
  add_library(spirvreflect
    ${spirvreflect_SOURCE_DIR}/spirv_reflect.h
    ${spirvreflect_SOURCE_DIR}/spirv_reflect.c
  )
  target_include_directories(spirvreflect PUBLIC
    ${spirvreflect_SOURCE_DIR}
  )
endif()

# mark SPIRVReflect as found
set(spirvreflect_FOUND TRUE)

# put SPIRVReflect under folder
set_target_properties(spirvreflect PROPERTIES FOLDER "Vendors")
