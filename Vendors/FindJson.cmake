include(FetchContent)

# define external project
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
)

# get properties
FetchContent_GetProperties(json)

# build json when needed
set(NLOHMANN_JSON_BUILD_MODULES OFF CACHE BOOL "" FORCE)
set(JSON_Install                OFF CACHE BOOL "" FORCE)
set(JSON_BuildTests             OFF CACHE BOOL "" FORCE)
set(JSON_CI                     OFF CACHE BOOL "" FORCE)
set(JSON_Diagnostics            ON  CACHE BOOL "" FORCE)
set(JSON_Diagnostic_Positions   ON  CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(json)

# mark json as found
set(json_FOUND TRUE)

# re-export target with namespace
add_library(json::json ALIAS nlohmann_json)

# move json under folder
set_target_properties(nlohmann_json PROPERTIES FOLDER "Vendors")
