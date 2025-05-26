add_library(lyra-common INTERFACE)
add_library(lyra::common ALIAS lyra-common)
target_include_directories(lyra-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
