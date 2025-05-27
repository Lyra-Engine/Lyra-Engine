# setup library
lyra_shared(common)

# library sources
target_sources(lyra-common PRIVATE

    # window sources
    Window/Types.cpp

    # render sources
    Render/RHI/Types.cpp
    Render/SLC/Types.cpp
)

# link dependencies
find_package(glm REQUIRED)
find_package(boxer REQUIRED)
find_package(spdlog REQUIRED)
target_link_libraries(lyra-common PUBLIC glm boxer spdlog)
target_include_directories(lyra-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
