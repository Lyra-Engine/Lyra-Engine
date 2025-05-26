# setup library
lyra_shared(common)

# library files
file(GLOB_RECURSE LYRA_COMMON_HDR ${CMAKE_CURRENT_SOURCE_DIR} *.h)
file(GLOB_RECURSE LYRA_COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# library sources
target_sources(lyra-common PRIVATE
    ${LYRA_COMMON_HDR}
    ${LYRA_COMMON_SRC}
)

# link dependencies
find_package(glm REQUIRED)
find_package(boxer REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(lyra-common PUBLIC glm boxer spdlog)
target_include_directories(lyra-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
