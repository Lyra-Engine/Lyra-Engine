include(FetchContent)

FetchContent_Declare(
  libassert
  GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
  GIT_TAG        v2.2.0 # <HASH or TAG>
)

FetchContent_MakeAvailable(libassert)

# mark libassert as found
set(libassert_FOUND TRUE)

# put libassert under folder
set_target_properties(libassert-lib PROPERTIES FOLDER "Vendors")
