#ifndef LYRA_LIBRARY_RENDER_SLC_DESCS_H
#define LYRA_LIBRARY_RENDER_SLC_DESCS_H

#include <Common/Container.h>
#include <Render/RHI/Enums.h>
#include <Render/SLC/Enums.h>
#include <Render/SLC/Utils.h>

namespace lyra::rhi
{

    struct CompilerDescriptor
    {
        CompileFlags              flags    = CompileFlag::NONE;
        CompileTarget             target   = CompileTarget::SPIRV;
        Vector<CString>           includes = {};
        HashMap<CString, CString> defines  = {};
    };

    struct CompileDescriptor
    {
        CString module;
        CString path;
        CString source;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_DESCS_H
