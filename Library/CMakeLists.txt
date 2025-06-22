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
target_link_libraries(lyra-engine PUBLIC glm::glm)
target_link_libraries(lyra-engine PUBLIC EnTT::EnTT)
target_link_libraries(lyra-engine PUBLIC boxer::boxer)
target_link_libraries(lyra-engine PUBLIC spdlog::spdlog)
target_link_libraries(lyra-engine PUBLIC libassert::assert)
target_link_libraries(lyra-engine PUBLIC magic_enum::magic_enum)

# include directories
# NOTE: cmake install target requires relative path.
# Use generator expression could avoid resolving relative path to absolute path.
target_include_directories(lyra-engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:./>
)
