include(FetchContent)

# define external project
FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v3.4.0
)

# get properties
FetchContent_GetProperties(tomlplusplus)

# build tomlplusplus when needed
set(TOMLPLUSPLUS_BUILD_MODULES OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES             OFF CACHE BOOL "" FORCE)
set(BUILD_FUZZER               OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(tomlplusplus)

# mark tomlplusplus as found
set(tomlplusplus_FOUND TRUE)

# re-export target with namespace
add_library(toml::toml ALIAS tomlplusplus_tomlplusplus)

# move tomlplusplus under folder
set_target_properties(tomlplusplus_tomlplusplus PROPERTIES FOLDER "Vendors")
