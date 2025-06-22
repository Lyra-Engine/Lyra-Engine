# setup library
lyra_shared(engine)

# library sources
target_sources(lyra-engine PRIVATE
    # common sources
    Lyra/Common/Msgbox.cpp

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

# directly exposed to user (with include headers)
target_link_libraries(lyra-engine PUBLIC
    $<BUILD_INTERFACE:glm::glm>
    $<BUILD_INTERFACE:EnTT::EnTT>
    $<BUILD_INTERFACE:spdlog::spdlog>
    $<BUILD_INTERFACE:libassert::assert>
    $<BUILD_INTERFACE:magic_enum::magic_enum>
)

# not directly exposed to user
target_link_libraries(lyra-engine PRIVATE boxer::boxer)

# include directories
# NOTE: cmake install target requires relative path.
# Use generator expression could avoid resolving relative path to absolute path.
target_include_directories(lyra-engine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:include>
)
