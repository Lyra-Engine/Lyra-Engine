#ifndef LYRA_PLUGIN_SLANG_UTILS_H
#define LYRA_PLUGIN_SLANG_UTILS_H

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

// both Slang and spdlog includes
#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Function.h>
#include <Lyra/Render/SLC/API.h>

using namespace lyra;
using namespace lyra::rhi;

auto get_logger() -> Logger;

enum class WalkAction
{
    SKIP,
    CONTINUE,
};

struct CumulativeOffset
{
    int value = 0; // the actual offset
    int space = 0; // the associated space
};

struct AccessPathNode
{
    slang::VariableLayoutReflection* layout = nullptr;
    AccessPathNode*                  outer  = nullptr;

    CumulativeOffset calculate_cumulative_offset() const;

    void print() const;
};

struct TraversalData
{
    uint current_walk_depth = 0;
};

struct TraverseDepthHandle
{
    TraverseDepthHandle(TraversalData& data) : data(data)
    {
        data.current_walk_depth++;
    }

    ~TraverseDepthHandle()
    {
        data.current_walk_depth--;
    }

    TraversalData& data;
};

struct CompileResultInternal
{
    Slang::ComPtr<slang::ISession>        session;
    Slang::ComPtr<slang::ICompileRequest> request;
    Slang::ComPtr<slang::IModule>         module;

    auto get_entry_point(CString entry) const -> Slang::ComPtr<slang::IEntryPoint>;
    auto get_linked_program(CString entry) const -> Slang::ComPtr<slang::IComponentType>;
    auto get_composed_program(CString entry) const -> Slang::ComPtr<slang::IComponentType>;

    bool get_shader_blob(CString entry, ShaderBlob& blob);
};

struct ReflectResultInternal
{
    using Bindings = TreeMap<uint, Vector<GPUBindGroupLayoutEntry>>;
    using Callback = std::function<WalkAction(AccessPathNode)>;
    using Metadata = std::pair<uint, slang::ICompileRequest*>;

    CompileTarget                     target;
    HashMap<GPUShaderStage, Metadata> metadata;
    HashMap<String, uint>             name2attributes;
    HashMap<String, uint>             name2bindgroups;
    Bindings                          bind_groups;
    Vector<GPUVertexAttribute>        vertex_attributes;
    TraversalData                     traversal_data;

    bool get_vertex_attributes(ShaderAttributes attrs, GPUVertexAttribute* attributes) const;
    bool get_bind_group_layouts(uint& count, GPUBindGroupLayoutDescriptor* layouts) const;
    bool get_bind_group_location(CString name, uint& group) const;

    void init(slang::ProgramLayout* program_layout);
    void walk(slang::EntryPointReflection* entry_point, AccessPathNode path, const Callback& callback);
    void walk(slang::VariableLayoutReflection* var_layout, AccessPathNode path, const Callback& callback);

    void init_bindings(slang::ProgramLayout* program_layout);
    void init_vertices(slang::ProgramLayout* program_layout);

    void record_parameter_block_space(AccessPathNode path);
    void create_binding(Bindings& bindings, AccessPathNode path);
    void create_automatic_constant_buffer(Bindings& bindings, AccessPathNode path);
    void fill_binding_type(slang::TypeLayoutReflection* type, GPUBindGroupLayoutEntry& entry) const;
    void fill_binding_count(slang::TypeLayoutReflection* type, GPUBindGroupLayoutEntry& entry) const;
    void fill_binding_stages(CumulativeOffset offset, GPUBindGroupLayoutEntry& entry) const;
    auto infer_texture_format(slang::TypeLayoutReflection* type) const -> GPUTextureFormat;
    auto infer_vertex_format(slang::TypeLayoutReflection* type) const -> GPUVertexFormat;
};

struct CompilerWrapper
{
    Slang::ComPtr<slang::ISession> session;

    static void init();

    explicit CompilerWrapper(const CompilerDescriptor& descriptor);

    auto select_profile(const CompilerDescriptor& descriptor) const -> SlangProfileID;

    auto select_target(const CompilerDescriptor& descriptor) const -> SlangCompileTarget;

    bool compile(const CompileDescriptor& desc, CompileResultInternal& result);

    bool reflect(ShaderEntryPoints entries, ReflectResultInternal& result);

    CompileTarget target;
};

#endif // LYRA_PLUGIN_SLANG_UTILS_H
