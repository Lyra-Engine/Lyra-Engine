#ifndef LYRA_LIBRARY_HPP
#define LYRA_LIBRARY_HPP

// Common headers
#include <Lyra/Common/Hash.h>
#include <Lyra/Common/Path.h>
#include <Lyra/Common/View.h>
#include <Lyra/Common/Enums.h>
#include <Lyra/Common/Assert.h>
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Msgbox.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Common/BitFlags.h>
#include <Lyra/Common/Function.h>
#include <Lyra/Common/Container.h>
#include <Lyra/Common/Blackboard.h>
#include <Lyra/Common/Compatibility.h>

// WSI (Window System Integration)
#include <Lyra/Window/WSIAPI.h>
#include <Lyra/Window/WSIEnums.h>
#include <Lyra/Window/WSIDescs.h>
#include <Lyra/Window/WSITypes.h>

// RHI (Render Hardware Interface)
#include <Lyra/Render/RHI/RHIAPI.h>
#include <Lyra/Render/RHI/RHIEnums.h>
#include <Lyra/Render/RHI/RHIDescs.h>
#include <Lyra/Render/RHI/RHITypes.h>
#include <Lyra/Render/RHI/RHIInits.h>

// RPI (Render Pass Interface)
#include <Lyra/Render/RPI/FrameGraph.h>
#include <Lyra/Render/RPI/FrameGraphPass.h>
#include <Lyra/Render/RPI/FrameGraphEnums.h>
#include <Lyra/Render/RPI/FrameGraphContext.h>
#include <Lyra/Render/RPI/FrameGraphBuilder.h>
#include <Lyra/Render/RPI/FrameGraphResource.h>

// SLC (Shader Language Compiler)
#include <Lyra/Shader/SLCAPI.h>
#include <Lyra/Shader/SLCEnums.h>
#include <Lyra/Shader/SLCDescs.h>
#include <Lyra/Shader/SLCTypes.h>

// GUI (Graphics User Interface)
#include <Lyra/GuiKit/GUIAPI.h>
#include <Lyra/GuiKit/GUITypes.h>

// FileIO
#include <Lyra/FileIO/FSAPI.h>
#include <Lyra/FileIO/FSEnums.h>
#include <Lyra/FileIO/FSTypes.h>

// AMS (Asset Management System)
#include <Lyra/Assets/AMSAPI.h>
#include <Lyra/Assets/AMSEnums.h>
#include <Lyra/Assets/AMSUtils.h>
#include <Lyra/Assets/AMSTypes.h>

// APP (Application)
#include <Lyra/AppKit/AppTypes.h>
#include <Lyra/AppKit/AppEnums.h>
#include <Lyra/AppKit/AppIcons.h>
#include <Lyra/AppKit/AppColors.h>

// Engine files
#include <Lyra/Engine/System/AssetManager.h>
#include <Lyra/Engine/System/ThemeManager.h>
#include <Lyra/Engine/System/LayoutManager.h>
#include <Lyra/Engine/System/ImGuiManager.h>
#include <Lyra/Engine/Assets/TextAsset.h>
#include <Lyra/Engine/Assets/JsonAsset.h>
#include <Lyra/Engine/Assets/TomlAsset.h>
#include <Lyra/Engine/Assets/TextureAsset.h>
#include <Lyra/Engine/Editor/Files.h>
#include <Lyra/Engine/Editor/Console.h>
#include <Lyra/Engine/Editor/Hierarchy.h>
#include <Lyra/Engine/Editor/SceneView.h>
#include <Lyra/Engine/Editor/Inspector.h>

#endif // LYRA_LIBRARY_HPP
