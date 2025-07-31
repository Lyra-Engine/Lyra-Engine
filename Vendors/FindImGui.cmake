include(FetchContent)

# define imgui project
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        34debc7 # use commit from docking branch
)

# get properties
FetchContent_GetProperties(imgui)

# populate imgui
if(NOT imgui_POPULATED)
  FetchContent_Populate(imgui)
endif()

# add imgui target (if not done so)
if(NOT TARGET imgui)
  add_library(imgui
    # imgui core sources
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  )

  # imgui headers
  target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
  )

  # alias
  add_library(imgui::imgui ALIAS imgui)
endif()

# mark imgui as found
set(imgui_FOUND TRUE)

# put imgui under folder
set_target_properties(imgui PROPERTIES PREFIX "")
set_target_properties(imgui PROPERTIES FOLDER "Vendors")
