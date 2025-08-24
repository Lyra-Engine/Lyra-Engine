# setup library
lyra_shared(engine)

# library sources
target_sources(lyra-engine PRIVATE
    # common sources
    Lyra/Common/Assert.h
    Lyra/Common/BitFlags.h
    Lyra/Common/Blackboard.h
    Lyra/Common/Compatibility.h
    Lyra/Common/Container.h
    Lyra/Common/ECS.h
    Lyra/Common/Enums.h
    Lyra/Common/Function.h
    Lyra/Common/GLM.h
    Lyra/Common/Hash.h
    Lyra/Common/Logger.h
    Lyra/Common/Msgbox.h
    Lyra/Common/Msgbox.cpp
    Lyra/Common/Path.h
    Lyra/Common/Plugin.h
    Lyra/Common/Pointer.h
    Lyra/Common/Slotmap.h
    Lyra/Common/Stdint.h
    Lyra/Common/String.h
    Lyra/Common/View.h

    # window sources
    Lyra/Window/WSIAPI.h
    Lyra/Window/WSIUtils.h
    Lyra/Window/WSIDescs.h
    Lyra/Window/WSIEnums.h
    Lyra/Window/WSIState.h
    Lyra/Window/WSIState.cpp
    Lyra/Window/WSIEvent.h
    Lyra/Window/WSITypes.h
    Lyra/Window/WSITypes.cpp

    # shader compiler sources
    Lyra/Shader/SLCAPI.h
    Lyra/Shader/SLCDescs.h
    Lyra/Shader/SLCEnums.h
    Lyra/Shader/SLCTypes.h
    Lyra/Shader/SLCTypes.cpp
    Lyra/Shader/SLCUtils.h

    # render sources
    Lyra/Render/RHIAPI.h
    Lyra/Render/RHIDescs.h
    Lyra/Render/RHIEnums.h
    Lyra/Render/RHIError.h
    Lyra/Render/RHIHash.h
    Lyra/Render/RHIInits.h
    Lyra/Render/RHIInits.cpp
    Lyra/Render/RHITypes.h
    Lyra/Render/RHITypes.cpp
    Lyra/Render/RHIUtils.h

    # rgpass sources
    Lyra/RPIKit/FrameGraph.h
    Lyra/RPIKit/FrameGraph.cpp
    Lyra/RPIKit/FrameGraphAllocator.h
    Lyra/RPIKit/FrameGraphAllocator.cpp
    Lyra/RPIKit/FrameGraphBuffer.h
    Lyra/RPIKit/FrameGraphBuilder.h
    Lyra/RPIKit/FrameGraphBuilder.cpp
    Lyra/RPIKit/FrameGraphEnums.h
    Lyra/RPIKit/FrameGraphPass.h
    Lyra/RPIKit/FrameGraphResource.h
    Lyra/RPIKit/FrameGraphTexture.h
    Lyra/RPIKit/FrameGraphTexture.cpp
    Lyra/RPIKit/FrameGraphTraits.h

    # guikit sources
    Lyra/GuiKit/GUIAPI.h
    Lyra/GuiKit/GUITypes.h
)

# link dependencies
find_package(glm REQUIRED)
find_package(fmt REQUIRED)
find_package(entt REQUIRED)
find_package(boxer REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(libassert REQUIRED)
find_package(magic_enum REQUIRED)

# directly exposed to user (with include headers)
target_link_libraries(lyra-engine PUBLIC
    $<BUILD_INTERFACE:glm::glm>
    $<BUILD_INTERFACE:EnTT::EnTT>
    $<BUILD_INTERFACE:imgui::imgui>
    $<BUILD_INTERFACE:fmt::fmt-header-only>
    $<BUILD_INTERFACE:spdlog::spdlog_header_only>
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
