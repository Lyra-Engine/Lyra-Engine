include(FetchContent)

# define external project
FetchContent_Declare(
  tinyusdz
  GIT_REPOSITORY https://github.com/syoyo/tinyusdz.git
  GIT_TAG        nested-variantset # v0.8.0rc4
)

# get properties
FetchContent_GetProperties(tinyusdz)

set(TINYUSDZ_BUILD_SHARED_LIBS         OFF CACHE BOOL "" FORCE)
set(TINYUSDZ_ENABLE_THREAD             ON  CACHE BOOL "" FORCE)
set(TINYUSDZ_BUILD_TESTS               OFF CACHE BOOL "" FORCE)
set(TINYUSDZ_BUILD_EXAMPLES            OFF CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_BUILTIN_IMAGE_LOADER OFF CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_USDMTLX              ON  CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_USDOBJ               ON  CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_USDFBX               ON  CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_USDVOX               ON  CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_OPENSUBDIV           OFF CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_AUDIO                ON  CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_ALAC_AUDIO           OFF CACHE BOOL "" FORCE)
set(TINYUSDZ_WITH_PYTHON               OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(tinyusdz)
set_property(TARGET tinyusdz_object PROPERTY CXX_STANDARD 14)
set_property(TARGET tinyusdz_static PROPERTY CXX_STANDARD 14)

# mark tinyusdz as found
set(tinyusdz_FOUND TRUE)
