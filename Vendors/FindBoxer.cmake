include(FetchContent)

# define external project
FetchContent_Declare(
  boxer
  GIT_REPOSITORY https://github.com/aaronmjacobs/Boxer.git
  GIT_TAG        master
)

# get properties
FetchContent_GetProperties(boxer)

# boxer's CMakeLists.txt does not provide an install option,
# therefore I have to unfortunately write one, and avoid
# directly using FetchContent_MakeAvailable

# populate boxer
if(NOT boxer_POPULATED)
  FetchContent_Populate(boxer)
endif()

# gather source files (based on operating system)
set(BOXER_SOURCES "")
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(APPEND BOXER_SOURCES "${boxer_SOURCE_DIR}/src/boxer_win.cpp")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND BOXER_SOURCES "${boxer_SOURCE_DIR}/src/boxer_linux.cpp")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  list(APPEND BOXER_SOURCES "${boxer_SOURCE_DIR}/src/boxer_mac.mm")
else()
  message(FATAL_ERROR "Boxer does not support unknown OS!")
endif()

# add boxer target
if(NOT TARGET boxer)
  add_library(boxer ${BOXER_SOURCES})
  target_include_directories(boxer PUBLIC
    $<BUILD_INTERFACE:${boxer_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
endif()

# mark boxer as found
set(boxer_FOUND TRUE)

# re-export target with namespace
add_library(boxer::boxer ALIAS boxer)

# move boxer under folder
set_target_properties(boxer PROPERTIES FOLDER "Vendors")

# add install rules for Boxer
install(TARGETS boxer
    EXPORT boxer-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${boxer_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT boxer-targets
    FILE boxer-targets.cmake
    NAMESPACE boxer::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boxer
)

# config file if you want to allow find_package(boxer)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/Targets/BoxerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/BoxerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boxer
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/BoxerConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Boxer
)
