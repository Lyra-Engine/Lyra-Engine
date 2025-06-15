# setup library
lyra_shared(core)

# library sources
target_sources(lyra-core PRIVATE

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
find_package(magic_enum REQUIRED)
target_link_libraries(lyra-core PUBLIC glm boxer spdlog magic_enum)
target_include_directories(lyra-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
