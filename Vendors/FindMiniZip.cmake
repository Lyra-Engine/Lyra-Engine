include(FetchContent)

# define external project
FetchContent_Declare(
  minizip
  GIT_REPOSITORY https://github.com/zlib-ng/minizip-ng.git
  GIT_TAG        4.0.10
)

# get properties
FetchContent_GetProperties(minizip)

# populate minizip
if(NOT minizip_POPULATED)
  FetchContent_Populate(minizip)
endif()

# build minizip when needed
set(MZ_BUILD_TESTS      OFF CACHE BOOL "" FORCE)
set(MZ_BUILD_FUZZ_TESTS OFF CACHE BOOL "" FORCE)
set(MZ_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
# set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(minizip)

# mark minizip as found
set(minizip_FOUND TRUE)

get_property(ALL_TARGETS GLOBAL PROPERTY BUILDSYSTEM_TARGETS)
foreach(TARGET_NAME IN LISTS ALL_TARGETS)
    message(STATUS "Found target: ${TARGET_NAME}")
endforeach()

# NOTE: Not finding a target called "minizip".
# move minizip under folder
set_target_properties(minizip PROPERTIES FOLDER "Vendors")
