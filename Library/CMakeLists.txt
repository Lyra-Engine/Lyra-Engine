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

    # render RHI sources
    Lyra/Render/RHI/RHIAPI.h
    Lyra/Render/RHI/RHIDescs.h
    Lyra/Render/RHI/RHIEnums.h
    Lyra/Render/RHI/RHIError.h
    Lyra/Render/RHI/RHIHash.h
    Lyra/Render/RHI/RHIInits.h
    Lyra/Render/RHI/RHIInits.cpp
    Lyra/Render/RHI/RHITypes.h
    Lyra/Render/RHI/RHITypes.cpp
    Lyra/Render/RHI/RHIUtils.h

    # render RPI sources
    Lyra/Render/RPI/FrameGraph.h
    Lyra/Render/RPI/FrameGraph.cpp
    Lyra/Render/RPI/FrameGraphAllocator.h
    Lyra/Render/RPI/FrameGraphAllocator.cpp
    Lyra/Render/RPI/FrameGraphBuffer.h
    Lyra/Render/RPI/FrameGraphBuilder.h
    Lyra/Render/RPI/FrameGraphBuilder.cpp
    Lyra/Render/RPI/FrameGraphEnums.h
    Lyra/Render/RPI/FrameGraphPass.h
    Lyra/Render/RPI/FrameGraphResource.h
    Lyra/Render/RPI/FrameGraphTexture.h
    Lyra/Render/RPI/FrameGraphTexture.cpp
    Lyra/Render/RPI/FrameGraphTraits.h

    # render GUI sources
    Lyra/Render/GUI/GUIAPI.h
    Lyra/Render/GUI/GUITypes.h
    Lyra/Render/GUI/GUITypes.cpp

    # shader compiler sources
    Lyra/Render/SLC/SLCAPI.h
    Lyra/Render/SLC/SLCDescs.h
    Lyra/Render/SLC/SLCEnums.h
    Lyra/Render/SLC/SLCTypes.h
    Lyra/Render/SLC/SLCTypes.cpp
    Lyra/Render/SLC/SLCUtils.h
)

# link dependencies
find_package(glm REQUIRED)
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
