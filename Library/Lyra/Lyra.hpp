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
#include <Lyra/Render/RHIAPI.h>
#include <Lyra/Render/RHIEnums.h>
#include <Lyra/Render/RHIDescs.h>
#include <Lyra/Render/RHITypes.h>
#include <Lyra/Render/RHIInits.h>

// RPI (Render Pass Interface)
#include <Lyra/RPIKit/FrameGraph.h>
#include <Lyra/RPIKit/FrameGraphPass.h>
#include <Lyra/RPIKit/FrameGraphEnums.h>
#include <Lyra/RPIKit/FrameGraphContext.h>
#include <Lyra/RPIKit/FrameGraphBuilder.h>
#include <Lyra/RPIKit/FrameGraphResource.h>

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

#endif // LYRA_LIBRARY_HPP
