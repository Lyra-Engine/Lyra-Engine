# setup library
lyra_shared(engine)

# library sources
target_sources(lyra-engine PRIVATE
    # render sources
    Lyra/Render/RHI/Types.cpp
    Lyra/Render/RHI/Inits.cpp
    Lyra/Render/RDG/Types.cpp
    Lyra/Render/SLC/Types.cpp

    # window sources
    Lyra/Window/Types.cpp
)

# link dependencies
find_package(glm REQUIRED)
find_package(entt REQUIRED)
find_package(boxer REQUIRED)
find_package(spdlog REQUIRED)
find_package(libassert REQUIRED)
find_package(magic_enum REQUIRED)
target_link_libraries(lyra-engine PUBLIC EnTT::EnTT)
target_link_libraries(lyra-engine PUBLIC libassert::assert)
target_link_libraries(lyra-engine PUBLIC glm boxer spdlog magic_enum)
target_include_directories(lyra-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
