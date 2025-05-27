#ifndef LYRA_LIBRARY_RENDER_SLC_TYPES_H
#define LYRA_LIBRARY_RENDER_SLC_TYPES_H

#include <Common/String.h>
#include <Common/Pointer.h>
#include <Render/RHI/Enums.h>
#include <Render/RHI/Descs.h>
#include <Render/SLC/Enums.h>
#include <Render/SLC/Utils.h>
#include <Render/SLC/Descs.h>

namespace lyra::rhi
{

    struct CompileResult
    {
        void*  res;
        String name;

        explicit CompileResult(void* res, CString entry);
        virtual ~CompileResult();

        auto size() const -> size_t;
        auto code() const -> ShaderBlob;
        auto error() const -> ShaderError;
        auto entry() const -> CString;

        void reflect(GPUPipelineLayoutDescriptor& desc) const;
    };

    struct Compiler
    {
        CompileTarget  target = CompileTarget::SPIRV;
        CompileFlags   flags  = CompileFlag::NONE;
        Vector<String> defines;
        Vector<String> includes;

        static Compiler init(const CompilerDescriptor& descriptor);

        void add_define(const String& macro);

        void add_include(const String& path);

        auto compile(GPUShaderStage stage, const String& entry, const String& source) -> Own<CompileResult>;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_SLC_TYPES_H
