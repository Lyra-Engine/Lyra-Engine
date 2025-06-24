# NOTE: Don't use vcpkg here, because libassert needs to be included in installation.

include(FetchContent)

FetchContent_Declare(
libassert
GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
GIT_TAG        v2.2.0 # <HASH or TAG>
)

set(CMAKE_SKIP_INSTALL_RULES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libassert)

# mark libassert as found
set(libassert_FOUND TRUE)

# put libassert under folder
set_target_properties(libassert-lib PROPERTIES FOLDER "Vendors")
set_target_properties(cpptrace-lib PROPERTIES FOLDER "Vendors")
