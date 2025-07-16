#include <iostream>
#include "SlangUtils.h"

static Slang::ComPtr<slang::IGlobalSession> GLOBAL_SESSION;

static Logger logger = init_stderr_logger("Slang", LogLevel::trace);

// #define SLANG_DEBUG
#ifdef SLANG_DEBUG
CString to_string(slang::TypeReflection::Kind kind)
{
    // clang-format off
    switch (kind) {
        case slang::TypeReflection::Kind::None:                 return "none";
        case slang::TypeReflection::Kind::Scalar:               return "scalar";
        case slang::TypeReflection::Kind::Vector:               return "vector";
        case slang::TypeReflection::Kind::Matrix:               return "matrix";
        case slang::TypeReflection::Kind::Array:                return "array";
        case slang::TypeReflection::Kind::Struct:               return "struct";
        case slang::TypeReflection::Kind::Resource:             return "resource";
        case slang::TypeReflection::Kind::SamplerState:         return "sampler_state";
        case slang::TypeReflection::Kind::TextureBuffer:        return "texture_buffer";
        case slang::TypeReflection::Kind::ShaderStorageBuffer:  return "shader_storage_buffer";
        case slang::TypeReflection::Kind::ParameterBlock:       return "parameter_block";
        case slang::TypeReflection::Kind::GenericTypeParameter: return "generic_type_parameter";
        case slang::TypeReflection::Kind::Interface:            return "interface";
        case slang::TypeReflection::Kind::OutputStream:         return "output_stream";
        case slang::TypeReflection::Kind::Specialized:          return "specialized";
        case slang::TypeReflection::Kind::Feedback:             return "feedback";
        case slang::TypeReflection::Kind::Pointer:              return "pointer";
        case slang::TypeReflection::Kind::ConstantBuffer:       return "constant_buffer";
        default:                                                return "unknown";
    }
    // clang-format on
}

CString to_string(slang::ParameterCategory category)
{
    // clang-format off
    switch (category) {
        case slang::ParameterCategory::None:                    return "none";
        case slang::ParameterCategory::Mixed:                   return "mixed";
        case slang::ParameterCategory::ConstantBuffer:          return "constant_buffer";
        case slang::ParameterCategory::ShaderResource:          return "shader_resource";
        case slang::ParameterCategory::UnorderedAccess:         return "unordered_access";
        case slang::ParameterCategory::VaryingInput:            return "varying_input";
        case slang::ParameterCategory::VaryingOutput:           return "varying_output";
        case slang::ParameterCategory::SamplerState:            return "sampler_state";
        case slang::ParameterCategory::Uniform:                 return "uniform";
        case slang::ParameterCategory::DescriptorTableSlot:     return "descriptor_table_slot";
        case slang::ParameterCategory::SpecializationConstant:  return "specialization_constant";
        case slang::ParameterCategory::PushConstantBuffer:      return "push_constant_buffer";
        case slang::ParameterCategory::RegisterSpace:           return "register_space";
        case slang::ParameterCategory::GenericResource:         return "generic_resource";
        case slang::ParameterCategory::RayPayload:              return "ray_payload";
        case slang::ParameterCategory::HitAttributes:           return "hit_attributes";
        case slang::ParameterCategory::CallablePayload:         return "callable_payload";
        case slang::ParameterCategory::ShaderRecord:            return "shader_record";
        case slang::ParameterCategory::ExistentialTypeParam:    return "existential_type_param";
        case slang::ParameterCategory::ExistentialObjectParam:  return "existential_object_param";
        case slang::ParameterCategory::MetalPayload:            return "metal_payload";
        case slang::ParameterCategory::MetalAttribute:          return "metal_attribute";
        case slang::ParameterCategory::SubElementRegisterSpace: return "subelement_register_space";
        default:                                                return "unknown";
    }
    // clang-format on
}

void print_slang_var_layout(slang::VariableLayoutReflection* var_layout, TraversalData& traversal)
{
    int  walk_depth  = traversal.current_walk_depth;
    auto print_depth = [&](CString prefix = "  ") -> std::ostream& {
        for (uint i = 0; i < walk_depth; i++)
            std::cout << prefix;
        return std::cout;
    };
    if (var_layout->getName())
        print_depth() << "# NAME: " << var_layout->getName() << std::endl;
    else
        print_depth() << "# NAME: unknwon" << std::endl;

    auto typ_layout = var_layout->getTypeLayout();
    print_depth() << "= type layout: " << to_string(typ_layout->getKind()) << std::endl;
    print_depth() << "= unit layout: " << to_string(typ_layout->getParameterCategory()) << std::endl;
    print_depth() << "= space: " << var_layout->getBindingSpace((SlangParameterCategory)typ_layout->getParameterCategory()) << std::endl;
    print_depth() << "= offset: " << var_layout->getOffset((SlangParameterCategory)typ_layout->getParameterCategory()) << std::endl;
}
#endif

Logger get_logger()
{
    return logger;
}

void diagnose_if_needed(slang::IBlob* diagnosticsBlob)
{
    if (diagnosticsBlob != nullptr) {
        get_logger()->info("Slang diagnostics: {}", (const char*)diagnosticsBlob->getBufferPointer());
    }
}

CumulativeOffset AccessPathNode::calculate_cumulative_offset() const
{
    CumulativeOffset result{};

    uint count = layout->getCategoryCount();
    for (uint i = 0; i < count; i++) {
        auto unit = static_cast<SlangParameterCategory>(layout->getCategoryByIndex(i));
        for (auto node = this; node != nullptr; node = node->outer) {
            result.value += node->layout->getOffset(unit);
            result.space += node->layout->getBindingSpace(unit);
            if (node->layout->getTypeLayout()->getKind() == slang::TypeReflection::Kind::ParameterBlock) {
                result.space += node->layout->getOffset(SLANG_PARAMETER_CATEGORY_SUB_ELEMENT_REGISTER_SPACE);
            }
        }
    }

    return result;
}

void AccessPathNode::print() const
{
    for (auto node = this; node != nullptr && node->layout != nullptr; node = node->outer) {
        if (node != this)
            std::cout << "<-";
        if (node->layout->getName())
            std::cout << node->layout->getName();
        else
            std::cout << "unknown";
        std::cout << " (" << node->layout << ") ";
    }
    std::cout << std::endl;
}

#pragma region CompilerWrapper
void CompilerWrapper::init()
{
    createGlobalSession(GLOBAL_SESSION.writeRef());
}

CompilerWrapper::CompilerWrapper(const CompilerDescriptor& descriptor)
{
    auto target_desc    = slang::TargetDesc{};
    target_desc.format  = select_target(descriptor);
    target_desc.profile = select_profile(descriptor);

    auto session_desc        = slang::SessionDesc{};
    session_desc.targets     = &target_desc;
    session_desc.targetCount = 1;

    Vector<slang::CompilerOptionEntry> options;

    // include dirs
    for (auto& include : descriptor.includes) {
        auto entry               = slang::CompilerOptionEntry{};
        entry.name               = slang::CompilerOptionName::Include;
        entry.value.kind         = slang::CompilerOptionValueKind::String;
        entry.value.stringValue0 = include;
        options.push_back(entry);
    }

    // preprocessor macros
    for (auto& macro : descriptor.defines) {
        auto entry               = slang::CompilerOptionEntry{};
        entry.name               = slang::CompilerOptionName::MacroDefine;
        entry.value.kind         = slang::CompilerOptionValueKind::String;
        entry.value.stringValue0 = macro.first;
        entry.value.stringValue1 = macro.second;
        options.push_back(entry);
    }

    // emit option
    auto emit = slang::CompilerOptionEntry{};
    {
        emit.name            = slang::CompilerOptionName::EmitSpirvDirectly;
        emit.value.kind      = slang::CompilerOptionValueKind::Int;
        emit.value.intValue0 = 1;
        options.push_back(emit);
    }

    GLOBAL_SESSION->createSession(session_desc, session.writeRef());
}

SlangProfileID CompilerWrapper::select_profile(const CompilerDescriptor& descriptor) const
{
    switch (descriptor.target) {
        case CompileTarget::DXIL:
            return GLOBAL_SESSION->findProfile("sm_6_5");
        case CompileTarget::SPIRV:
        default: // fallback for invalid arguments
            return GLOBAL_SESSION->findProfile("spirv_1_5");
    }
}

SlangCompileTarget CompilerWrapper::select_target(const CompilerDescriptor& descriptor) const
{
    switch (descriptor.target) {
        case CompileTarget::DXIL:
            return SLANG_DXIL;
        case CompileTarget::SPIRV:
        default:
            return SLANG_SPIRV;
    }
}

bool CompilerWrapper::compile(const CompileDescriptor& desc, CompileResultInternal& result)
{
    // create shader module
    {
        Slang::ComPtr<slang::IBlob> diagnostics;
        result.module = session->loadModuleFromSourceString(
            desc.module,             // module name
            desc.path,               // module path
            desc.source,             // shader source code
            diagnostics.writeRef()); // optional diagnostic container
        diagnose_if_needed(diagnostics);
    }

    result.session = session;
    return result.module != nullptr;
}

bool CompilerWrapper::reflect(ShaderEntryPoints entries, ReflectResultInternal& result)
{
    // make sure single module is added exactly once
    HashSet<slang::IComponentType*> modules;
    for (auto& entry : entries) {
        auto module = reinterpret_cast<CompileResultInternal*>(entry.module.handle);
        modules.insert(module->module);
    }

    // find all required shader entry points
    Vector<slang::IComponentType*>            component_types(modules.begin(), modules.end());
    Vector<Slang::ComPtr<slang::IEntryPoint>> entry_points;
    for (auto& entry : entries) {
        auto module = reinterpret_cast<CompileResultInternal*>(entry.module.handle);
        auto entryp = module->get_entry_point(entry.entry);
        entry_points.push_back(entryp);
        component_types.push_back(entryp);
    }

    // create a composed program for reflection
    Slang::ComPtr<slang::IComponentType> composed_program;
    {
        Slang::ComPtr<slang::IBlob> diagnostics;

        SlangResult result = session->createCompositeComponentType(
            component_types.data(),
            component_types.size(),
            composed_program.writeRef(),
            diagnostics.writeRef());
        diagnose_if_needed(diagnostics);
        SLANG_RETURN_FALSE_ON_FAIL(result);
    }

    // link the program
    Slang::ComPtr<slang::IComponentType> linked_program;
    {
        Slang::ComPtr<slang::IBlob> diagnostics;

        SlangResult result = composed_program->link(
            linked_program.writeRef(),
            diagnostics.writeRef());
        diagnose_if_needed(diagnostics);
        SLANG_RETURN_FALSE_ON_FAIL(result);
    }
    result.init(linked_program->getLayout());
    return true;
}
#pragma endregion CompilerWrapper

#pragma region CompileResultInternal
Slang::ComPtr<slang::IEntryPoint> CompileResultInternal::get_entry_point(CString entry) const
{
    // query entry point
    Slang::ComPtr<slang::IEntryPoint> entry_point;
    Slang::ComPtr<slang::IBlob>       diagnostics;
    module->findEntryPointByName(entry, entry_point.writeRef());

    if (!entry_point) {
        get_logger()->info("Failed to get entry point: {}", entry);
        return nullptr;
    }
    return entry_point;
}

Slang::ComPtr<slang::IComponentType> CompileResultInternal::get_linked_program(CString entry) const
{
    auto composed_program = get_composed_program(entry);
    if (composed_program == nullptr)
        return nullptr;

    // link
    Slang::ComPtr<slang::IComponentType> linked_program;
    Slang::ComPtr<slang::IBlob>          diagnostics;

    SlangResult result = composed_program->link(
        linked_program.writeRef(),
        diagnostics.writeRef());
    diagnose_if_needed(diagnostics);

    if (SLANG_FAILED(result)) {
        get_logger()->info("Failed to link program: {}", entry);
        return nullptr;
    }
    return linked_program;
}

Slang::ComPtr<slang::IComponentType> CompileResultInternal::get_composed_program(CString entry) const
{
    auto entry_point = get_entry_point(entry);
    if (entry_point == nullptr)
        return nullptr;

    // compose modules + entry points
    std::array<slang::IComponentType*, 2> component_types = {module, entry_point};
    Slang::ComPtr<slang::IComponentType>  composed_program;
    Slang::ComPtr<slang::IBlob>           diagnostics;

    SlangResult result = session->createCompositeComponentType(
        component_types.data(),
        component_types.size(),
        composed_program.writeRef(),
        diagnostics.writeRef());
    diagnose_if_needed(diagnostics);

    if (SLANG_FAILED(result)) {
        get_logger()->info("Failed to compose program: {}", entry);
        return nullptr;
    }
    return composed_program;
}

bool CompileResultInternal::get_shader_blob(CString entry, ShaderBlob& blob)
{
    auto linked_program = get_linked_program(entry);
    if (linked_program == nullptr)
        return false;

    // kernel binary
    Slang::ComPtr<slang::IBlob> spirv_code;
    Slang::ComPtr<slang::IBlob> diagnostics;

    SlangResult result = linked_program->getEntryPointCode(
        0,
        0,
        spirv_code.writeRef(),
        diagnostics.writeRef());
    diagnose_if_needed(diagnostics);
    SLANG_RETURN_ON_FAIL(result);

    blob.size = static_cast<uint32_t>(spirv_code->getBufferSize());
    blob.data = new uint8_t[blob.size];
    std::memcpy((uint8_t*)blob.data, (uint8_t*)spirv_code->getBufferPointer(), blob.size);
    return true;
}
#pragma endregion CompileResultInternal

#pragma region ReflectResultInternal
GPUVertexAttributes ReflectResultInternal::get_vertex_attributes() const
{
    return attributes;
}

GPUBindGroupLayoutDescriptors ReflectResultInternal::get_bind_group_layouts() const
{
    return bind_group_layouts;
}

bool ReflectResultInternal::get_bind_group_location(CString name, uint& group, uint& binding) const
{
    return false;
}

void ReflectResultInternal::init(slang::ProgramLayout* program_layout)
{
    SetBindings bindings;

    auto callback = [&](AccessPathNode node) {
        auto typ_layout = node.layout->getTypeLayout();
        switch (typ_layout->getKind()) {
            case slang::TypeReflection::Kind::ParameterBlock:
                // ParameterBlock has a name, we record it for bind group retrieval
                record_parameter_block_space(node);

                // ParameterBlock will automatically introduce a constant buffer binding if ordinary types are observed.
                if (typ_layout->getSize())
                    this->create_automatic_constant_buffer(bindings, node);

                return WalkAction::CONTINUE;
            case slang::TypeReflection::Kind::Resource:
            case slang::TypeReflection::Kind::SamplerState:
            case slang::TypeReflection::Kind::TextureBuffer:
            case slang::TypeReflection::Kind::ConstantBuffer:
            case slang::TypeReflection::Kind::ShaderStorageBuffer:
                this->create_binding(bindings, node);
                return WalkAction::SKIP;
            default:
                return WalkAction::CONTINUE;
        }
    };

    // reset traversal data
    traversal_data.current_walk_depth = 0;

    // iterate through global parameters
    // get_logger()->info("############## VISIT GLOBAL PARAMS ##############");
    auto global_params = program_layout->getGlobalParamsVarLayout();
    walk(global_params, {}, callback);

    // iterate through entry points
    uint entry_point_count = program_layout->getEntryPointCount();
    for (uint i = 0; i < entry_point_count; i++) {
        // get_logger()->info("############## VISIT ENTRY POINT ##############");
        auto entry_point_reflect = program_layout->getEntryPointByIndex(i);
        walk(entry_point_reflect, {}, callback);
    }
}

void ReflectResultInternal::walk(slang::EntryPointReflection* entry_point, AccessPathNode path, const Callback& callback)
{
    auto var_layout = entry_point->getVarLayout();
    auto path_node  = AccessPathNode{var_layout, &path};
    walk(var_layout, path_node, callback);
}

void ReflectResultInternal::walk(slang::VariableLayoutReflection* var_layout, AccessPathNode path, const Callback& callback)
{
    auto typ_layout = var_layout->getTypeLayout();

#ifdef SLANG_DEBUG
    print_slang_var_layout(var_layout, traversal_data);

    // a handle that automatically increment/decrement the traversal depth
    TraverseDepthHandle depth_walker(traversal_data);
#endif

    // invoke callback
    auto path_node = AccessPathNode{var_layout, &path};
    if (callback(path_node) == WalkAction::SKIP)
        return;

    // special case for parameter block
    if (typ_layout->getKind() == slang::TypeReflection::Kind::ParameterBlock) {
        auto inner = typ_layout->getElementVarLayout();
        return walk(inner, path_node, callback);
    }

    // traverse
    uint field_count = typ_layout->getFieldCount();
    for (uint i = 0; i < field_count; i++) {
        auto field_var = typ_layout->getFieldByIndex(i);
        walk(field_var, path_node, callback);
    }
}

void ReflectResultInternal::record_parameter_block_space(AccessPathNode path)
{
    assert(path.layout->getTypeLayout()->getKind() == slang::TypeReflection::Kind::ParameterBlock);

    auto name   = path.layout->getName();
    auto offset = path.calculate_cumulative_offset();
    spaces.emplace(name, offset.space);
}

void ReflectResultInternal::create_automatic_constant_buffer(SetBindings& bindings, AccessPathNode node)
{
    auto offset = node.calculate_cumulative_offset();

    uint space   = offset.space;
    uint binding = offset.value;

    auto val    = GPUBindGroupLayoutEntry{};
    val.binding = binding; // binding index within the space
    val.type    = GPUBindingResourceType::BUFFER;
    val.count   = 1;
    fill_binding_stages(offset, val);

    bindings[space].push_back(val);
    get_logger()->info("NAME:{}\t SPACE:{} BINDING:{} (AUTOMATIC)", node.layout->getName(), space, binding);
}

void ReflectResultInternal::create_binding(SetBindings& bindings, AccessPathNode node)
{
    auto type   = node.layout->getTypeLayout();
    auto offset = node.calculate_cumulative_offset();

    uint space   = offset.space;
    uint binding = offset.value;

    auto val    = GPUBindGroupLayoutEntry{};
    val.binding = binding; // binding index within the space
    fill_binding_type(type, val);
    fill_binding_count(type, val);
    fill_binding_stages(offset, val);

    // append to bindings
    bindings[space].push_back(val);
    get_logger()->info("NAME:{}\t SPACE:{} BINDING:{}", node.layout->getName(), space, binding);
}

void ReflectResultInternal::fill_binding_count(slang::TypeLayoutReflection* type, GPUBindGroupLayoutEntry& entry) const
{
    // entry.bindless = false;

    if (!type->isArray()) {
        entry.count = 1;
        return;
    }

    // possibly unbound
    size_t count = type->getTotalArrayElementCount();
    // if (count == ~size_t(0))
    //     entry.bindless = true;

    entry.count = count;
}

void ReflectResultInternal::fill_binding_stages(CumulativeOffset offset, GPUBindGroupLayoutEntry& entry) const
{
    // NOTE: implement this using IMetadata
    entry.visibility = GPUShaderStage::VERTEX | GPUShaderStage::FRAGMENT;
}

void ReflectResultInternal::fill_binding_type(slang::TypeLayoutReflection* type, GPUBindGroupLayoutEntry& entry) const
{
    auto kind = type->getKind();

    // common stuff
    auto view_dimension    = GPUTextureViewDimension::x1D;
    auto access_permission = GPUStorageTextureAccess::READ_ONLY;

    switch (kind) {
        case slang::TypeReflection::Kind::Resource:
        {
            auto shape  = type->getResourceShape();
            auto access = type->getResourceAccess();

            // view dimension
            // clang-format off
            switch (shape & SLANG_RESOURCE_BASE_SHAPE_MASK) {
                case SLANG_TEXTURE_1D:         view_dimension = GPUTextureViewDimension::x1D;        break;
                case SLANG_TEXTURE_2D:         view_dimension = GPUTextureViewDimension::x2D;        break;
                case SLANG_TEXTURE_2D_ARRAY:   view_dimension = GPUTextureViewDimension::x2D_ARRAY;  break;
                case SLANG_TEXTURE_3D:         view_dimension = GPUTextureViewDimension::x3D;        break;
                case SLANG_TEXTURE_CUBE:       view_dimension = GPUTextureViewDimension::CUBE;       break;
                case SLANG_TEXTURE_CUBE_ARRAY: view_dimension = GPUTextureViewDimension::CUBE_ARRAY; break;
                case SLANG_TEXTURE_1D_ARRAY:   assert(!!!"TEXTURE1D_ARRAY is not supported!");       break;
                default:                                                                             break;
            }
            // clang-format on

            // texture access
            // clang-format off
            switch (access) {
                case SLANG_RESOURCE_ACCESS_READ:       access_permission = GPUStorageTextureAccess::READ_ONLY;  break;
                case SLANG_RESOURCE_ACCESS_WRITE:      access_permission = GPUStorageTextureAccess::WRITE_ONLY; break;
                case SLANG_RESOURCE_ACCESS_READ_WRITE: access_permission = GPUStorageTextureAccess::READ_WRITE; break;
                default:                                                                                        break;
            }
            // clang-format on

            switch (shape & SLANG_RESOURCE_BASE_SHAPE_MASK) {
                case SLANG_TEXTURE_1D:
                case SLANG_TEXTURE_2D:
                case SLANG_TEXTURE_3D:
                case SLANG_TEXTURE_CUBE:
                    entry.type = (access_permission != GPUStorageTextureAccess::READ_ONLY)
                                     ? GPUBindingResourceType::STORAGE_TEXTURE
                                     : GPUBindingResourceType::TEXTURE;
                    if (entry.type == GPUBindingResourceType::STORAGE_TEXTURE) {
                        entry.storage_texture.view_dimension = view_dimension;
                        entry.storage_texture.access         = access_permission;
                        entry.storage_texture.format         = infer_texture_format(type);
                    } else {
                        entry.texture.view_dimension = view_dimension;
                        entry.texture.multisampled   = shape & SLANG_TEXTURE_MULTISAMPLE_FLAG;
                        entry.texture.sample_type    = GPUTextureSampleType::FLOAT;
                    }
                    return;
                case SLANG_STRUCTURED_BUFFER:
                case SLANG_BYTE_ADDRESS_BUFFER:
                    entry.type                      = GPUBindingResourceType::BUFFER;
                    entry.buffer.type               = (access_permission != GPUStorageTextureAccess::READ_ONLY)
                                                          ? GPUBufferBindingType::READ_ONLY_STORAGE
                                                          : GPUBufferBindingType::STORAGE;
                    entry.buffer.has_dynamic_offset = false;
                    entry.buffer.min_binding_size   = type->getSize();
                    return;
                case SLANG_ACCELERATION_STRUCTURE:
                    entry.type              = GPUBindingResourceType::ACCELERATION_STRUCTURE;
                    entry.bvh.vertex_return = false;
                    return;
                default:
                    entry.type = GPUBindingResourceType::TEXTURE;
                    return;
            }
        }

        case slang::TypeReflection::Kind::SamplerState:
            entry.type         = GPUBindingResourceType::SAMPLER;
            entry.sampler.type = GPUSamplerBindingType::FILTERING;
            return;

        case slang::TypeReflection::Kind::TextureBuffer:
        case slang::TypeReflection::Kind::ShaderStorageBuffer:
            entry.type        = GPUBindingResourceType::BUFFER;
            entry.buffer.type = GPUBufferBindingType::STORAGE;
            return;
        case slang::TypeReflection::Kind::ConstantBuffer:
        default:
            entry.type                    = GPUBindingResourceType::BUFFER;
            entry.buffer.type             = GPUBufferBindingType::UNIFORM;
            entry.buffer.min_binding_size = type->getSize();
            return;
    }
}

GPUTextureFormat ReflectResultInternal::infer_texture_format(slang::TypeLayoutReflection* type) const
{
    auto kind = type->getKind();
    if (kind != slang::TypeReflection::Kind::Resource) {
        assert(!!!"Failed to infer texture format!");
        return GPUTextureFormat::RGBA32FLOAT;
    }

    // get the element type of the resource
    auto element_type = type->getElementTypeLayout();
    if (!element_type) {
        assert(!!!"Failed to infer texture format!");
        return GPUTextureFormat::RGBA32FLOAT;
    }

    // check the scalar type and component count
    auto scalar_type     = element_type->getScalarType();
    auto component_count = element_type->getElementCount();

    // map to common formats
    // clang-format off
    switch (scalar_type) {
        case slang::TypeReflection::ScalarType::Float32:
            switch (component_count) {
                case 1: return GPUTextureFormat::R32FLOAT;
                case 2: return GPUTextureFormat::RG32FLOAT;
                case 4: return GPUTextureFormat::RGBA32FLOAT;
                default: assert(!!!"unsupported channel count!");
            }
            break;

        case slang::TypeReflection::ScalarType::UInt32:
            switch (component_count) {
                case 1: return GPUTextureFormat::R32UINT;
                case 2: return GPUTextureFormat::RG32UINT;
                case 4: return GPUTextureFormat::RGBA32UINT;
                default: assert(!!!"unsupported channel count!");
            }
            break;

        case slang::TypeReflection::ScalarType::Int32:
            switch (component_count) {
                case 1: return GPUTextureFormat::R32SINT;
                case 2: return GPUTextureFormat::RG32SINT;
                case 4: return GPUTextureFormat::RGBA32SINT;
                default: assert(!!!"unsupported channel count!");
            }
            break;

        case slang::TypeReflection::ScalarType::UInt16:
            switch (component_count) {
                case 1: return GPUTextureFormat::R16UINT;
                case 2: return GPUTextureFormat::RG16UINT;
                case 4: return GPUTextureFormat::RGBA16UINT;
                default: assert(!!!"unsupported channel count!");
            }
            break;

        case slang::TypeReflection::ScalarType::Int16:
            switch (component_count) {
                case 1: return GPUTextureFormat::R16SINT;
                case 2: return GPUTextureFormat::RG16SINT;
                case 4: return GPUTextureFormat::RGBA16SINT;
                default: assert(!!!"unsupported channel count!");
            }
            break;

        case slang::TypeReflection::ScalarType::UInt8:
            switch (component_count) {
                case 1: return GPUTextureFormat::R8UINT;
                case 2: return GPUTextureFormat::RG8UINT;
                case 4: return GPUTextureFormat::RGBA8UINT;
                default: assert(!!!"unsupported channel count!");
            }
            break;

        case slang::TypeReflection::ScalarType::Int8:
            switch (component_count) {
                case 1: return GPUTextureFormat::R8SINT;
                case 2: return GPUTextureFormat::RG8SINT;
                case 4: return GPUTextureFormat::RGBA8SINT;
                default: assert(!!!"unsupported channel count!");
            }
            break;

        default: break;
    }
    // clang-format on

    assert(!!!"Failed to infer texture format!");
    return GPUTextureFormat::RGBA32FLOAT;
}
#pragma endregion ReflectResultInternal
