# NOTE: Don't use vcpkg here, because fmt needs to be included in installation.

include(FetchContent)

# define external project
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        11.2.0
)

# get properties
FetchContent_GetProperties(fmt)

# build fmt when needed (enable install)
FetchContent_MakeAvailable(fmt)

# mark fmt as found
set(fmt_FOUND TRUE)

set_target_properties(fmt PROPERTIES FOLDER "Vendors")
