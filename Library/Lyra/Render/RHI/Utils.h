#ifndef LYRA_LIBRARY_RENDER_RHI_UTILS_H
#define LYRA_LIBRARY_RENDER_RHI_UTILS_H

#include <Lyra/Common/View.h>
#include <Lyra/Common/Assert.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Handle.h>
#include <Lyra/Common/BitFlags.h>
#include <Lyra/Render/RHI/Enums.h>

ENABLE_BIT_FLAGS(lyra::rhi::RHIFlag);
ENABLE_BIT_FLAGS(lyra::rhi::GPUShaderStage);
ENABLE_BIT_FLAGS(lyra::rhi::GPUBufferUsage);
ENABLE_BIT_FLAGS(lyra::rhi::GPUTextureUsage);
ENABLE_BIT_FLAGS(lyra::rhi::GPUColorWrite);
ENABLE_BIT_FLAGS(lyra::rhi::GPUBarrierSync);
ENABLE_BIT_FLAGS(lyra::rhi::GPUBarrierAccess);

namespace lyra::rhi
{
    using BufferSource = uint8_t*;

    using GPUBufferDynamicOffset   = uint32_t;
    using GPUStencilValue          = uint32_t;
    using GPUSampleMask            = uint32_t;
    using GPUDepthBias             = int32_t;
    using GPUSize64                = uint64_t;
    using GPUIntegerCoordinate     = uint32_t;
    using GPUIndex32               = uint32_t;
    using GPUSize32                = uint32_t;
    using GPUSignedOffset32        = int32_t;
    using GPUSize64Out             = uint64_t;
    using GPUIntegerCoordinateOut  = uint32_t;
    using GPUSize32Out             = uint32_t;
    using GPUFlagsConstant         = uint32_t;
    using GPUPipelineConstantValue = uint64_t;
    using GPUBufferAddress         = uint64_t;
    using RHIFlags                 = BitFlags<RHIFlag>;
    using GPUShaderStageFlags      = BitFlags<GPUShaderStage>;
    using GPUBufferUsageFlags      = BitFlags<GPUBufferUsage>;
    using GPUTextureUsageFlags     = BitFlags<GPUTextureUsage>;
    using GPUColorWriteFlags       = BitFlags<GPUColorWrite>;
    using GPUBarrierSyncFlags      = BitFlags<GPUBarrierSync>;
    using GPUBarrierAccessFlags    = BitFlags<GPUBarrierAccess>;
    using GPUBVHFlags              = BitFlags<GPUBVHFlag>;
    using GPUBVHGeometryFlags      = BitFlags<GPUBVHGeometryFlag>;
    using GPUTextureAspectFlags    = BitFlags<GPUTextureAspect>;

    template <GPUObjectType E>
    using GPUHandle = Handle<GPUObjectType, E>;

    // typed GPU handle
    using GPUFenceHandle              = GPUHandle<GPUObjectType::FENCE>;
    using GPUCommandEncoderHandle     = GPUHandle<GPUObjectType::COMMAND_ENCODER>;
    using GPUBufferHandle             = GPUHandle<GPUObjectType::BUFFER>;
    using GPUSamplerHandle            = GPUHandle<GPUObjectType::SAMPLER>;
    using GPUTextureHandle            = GPUHandle<GPUObjectType::TEXTURE>;
    using GPUTextureViewHandle        = GPUHandle<GPUObjectType::TEXTURE_VIEW>;
    using GPUShaderModuleHandle       = GPUHandle<GPUObjectType::SHADER_MODULE>;
    using GPUQuerySetHandle           = GPUHandle<GPUObjectType::QUERY_SET>;
    using GPUTlasHandle               = GPUHandle<GPUObjectType::TLAS>;
    using GPUBlasHandle               = GPUHandle<GPUObjectType::BLAS>;
    using GPUBindGroupHandle          = GPUHandle<GPUObjectType::BIND_GROUP>;
    using GPUBindGroupLayoutHandle    = GPUHandle<GPUObjectType::BIND_GROUP_LAYOUT>;
    using GPUPipelineLayoutHandle     = GPUHandle<GPUObjectType::PIPELINE_LAYOUT>;
    using GPURenderPipelineHandle     = GPUHandle<GPUObjectType::RENDER_PIPELINE>;
    using GPUComputePipelineHandle    = GPUHandle<GPUObjectType::COMPUTE_PIPELINE>;
    using GPURayTracingPipelineHandle = GPUHandle<GPUObjectType::RAYTRACING_PIPELINE>;

    // forward declarations
    using GPUFeatureNames   = TypedView<GPUFeatureName>;
    using GPUTextureFormats = TypedView<GPUTextureFormat>;

    struct GPUTlasInstance;
    using GPUTlasInstances = TypedView<GPUTlasInstance>;

    struct GPUBlasTriangleGeometry;
    using GPUBlasTriangleGeometries = TypedView<GPUBlasTriangleGeometry>;

    struct GPUBindGroupEntry;
    using GPUBindGroupEntries = TypedView<GPUBindGroupEntry>;

    struct GPUBindGroupLayoutEntry;
    using GPUBindGroupLayoutEntries = TypedView<GPUBindGroupLayoutEntry>;
    using GPUBindGroupLayoutHandles = TypedView<GPUBindGroupLayoutHandle>;

    struct GPUBindGroupLayoutDescriptor;
    using GPUBindGroupLayoutDescriptors = TypedView<GPUBindGroupLayoutDescriptor>;

    struct GPUVertexAttribute;
    using GPUVertexAttributes = TypedView<GPUVertexAttribute>;

    struct GPUVertexBufferLayout;
    using GPUVertexBufferLayouts = TypedView<GPUVertexBufferLayout>;

    struct GPUColorTargetState;
    using GPUColorTargetStates = TypedView<GPUColorTargetState>;

    struct GPURenderPassColorAttachment;
    using GPURenderPassColorAttachments = TypedView<GPURenderPassColorAttachment>;

    struct GPUBlasGeometrySizeDescriptor;
    using GPUBlasGeometrySizeDescriptors = TypedView<GPUBlasGeometrySizeDescriptor>;

    struct GPUTlasBuildEntry;
    using GPUTlasBuildEntries = TypedView<GPUTlasBuildEntry>;

    struct GPUBlasBuildEntry;
    using GPUBlasBuildEntries = TypedView<GPUBlasBuildEntry>;

    struct GPUMemoryBarrier;
    using GPUMemoryBarriers = TypedView<GPUMemoryBarrier>;

    struct GPUBufferBarrier;
    using GPUBufferBarriers = TypedView<GPUBufferBarrier>;

    struct GPUTextureBarrier;
    using GPUTextureBarriers = TypedView<GPUTextureBarrier>;

    struct GPUBufferBinding;
    using GPUBufferBindings     = TypedView<GPUBufferBinding>;
    using GPUSamplerHandles     = TypedView<GPUSamplerHandle>;
    using GPUTextureViewHandles = TypedView<GPUTextureViewHandle>;

    using GPUBufferDynamicOffsets = TypedView<GPUBufferDynamicOffset>;

    struct MappedBufferRange
    {
        BufferSource data;
        size_t       size;
    };

    template <typename T>
    struct TypedBufferRange
    {
        T*     data;
        size_t count;

        auto operator[](size_t i) -> T&
        {
            return data[i];
        }

        auto operator[](size_t i) const -> T&
        {
            return data[i];
        }

        auto at(size_t i) -> T&
        {
            assert(i < count);
            return data[i];
        }

        auto at(size_t i) const -> T&
        {
            assert(i < count);
            return data[i];
        }
    };

    // NOTE: Non-WebGPU standard
    struct GPUBindingIndex
    {
        // index is the same as Vulkan style flattened binding,
        // expected to be unique within the bind group.
        uint16_t index = 0;

        // register_type and register_index are D3D12 specific fields,
        // because D3D12 uses spaced registers instead of flattened indexed bindings.
        // Therefore, for robust bind group layout creation, it is necessary to specify
        // both binding index and register* info.
        // Users are expected to use the reflection or serialization/deserialization API
        // to automatically populate these.
        uint16_t register_index = 0;
    };

    struct GPUSupportedFeatures
    {
        bool bgra8unorm_storage                 = false;
        bool clip_distances                     = false;
        bool depth_clip_control                 = false;
        bool depth32float_stencil8              = false;
        bool dual_source_blending               = false;
        bool float32_blendable                  = false;
        bool float32_filterable                 = false;
        bool indirect_first_instance            = false;
        bool rg11b10ufloat_renderable           = false;
        bool shader_f16                         = false;
        bool subgroups                          = false;
        bool texture_compression_bc             = false;
        bool texture_compression_bc_sliced_3d   = false;
        bool texture_compression_astc           = false;
        bool texture_compression_astc_sliced_3d = false;
        bool texture_compression_etc2           = false;
        bool timestamp_query                    = false;
        bool bindless                           = false;
        bool raytracing                         = false;
    };

    struct GPUSupportedLimits
    {
        uint max_texture_dimension_1d                        = 8192;
        uint max_texture_dimension_2d                        = 8192;
        uint max_texture_dimension_3d                        = 2048;
        uint max_texture_array_layers                        = 256;
        uint max_bind_groups                                 = 4;
        uint max_bindings_per_bind_group                     = 640;
        uint max_dynamic_uniform_buffers_per_pipeline_layout = 8;
        uint max_dynamic_storage_buffers_per_pipeline_layout = 4;
        uint max_sampled_textures_per_shader_stage           = 16;
        uint max_samplers_per_shader_stage                   = 16;
        uint max_storage_buffers_per_shader_stage            = 8;
        uint max_storage_textures_per_shader_stage           = 4;
        uint max_uniform_buffers_per_shader_stage            = 12;
        uint max_uniform_buffer_binding_size                 = 65536;
        uint max_storage_buffer_binding_size                 = 134217728;
        uint min_uniform_buffer_offset_alignment             = 256;
        uint min_storage_buffer_offset_alignment             = 256;
        uint max_vertex_buffers                              = 8;
        uint max_buffer_size                                 = 268435456;
        uint max_vertex_attributes                           = 16;
        uint max_vertex_bufferArrayStride                    = 2048;
        uint max_inter_stage_shader_variables                = 16;
        uint max_color_attachments                           = 8;
        uint max_color_attachment_bytes_per_sample           = 32;
        uint max_compute_workgroup_storage_size              = 16384;
        uint max_compute_invocations_per_workgroup           = 256;
        uint max_compute_workgroup_size_x                    = 256;
        uint max_compute_workgroup_size_y                    = 256;
        uint max_compute_workgroup_size_z                    = 64;
        uint max_compute_workgroups_per_dimension            = 65535;
    };

    struct GPUProperties
    {
        uint subgroup_max_size           = 0;
        uint subgroup_min_size           = 0;
        uint texture_row_pitch_alignment = 0;
    };

    struct GPUAdapterInfo
    {
        String architecture = "";
        String descrition   = "";
        String device       = "";
        String vendor       = "";
    };

    struct GPUColor
    {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 0.0f;
    };

    struct GPUOrigin2D
    {
        GPUIntegerCoordinate x = 0;
        GPUIntegerCoordinate y = 0;
    };

    struct GPUOrigin3D
    {
        GPUIntegerCoordinate x = 0;
        GPUIntegerCoordinate y = 0;
        GPUIntegerCoordinate z = 0;
    };

    struct GPUExtent2D
    {
        GPUIntegerCoordinate width  = 0;
        GPUIntegerCoordinate height = 0;
    };

    struct GPUExtent3D
    {
        GPUIntegerCoordinate width  = 0;
        GPUIntegerCoordinate height = 0;
        GPUIntegerCoordinate depth  = 0;
    };

    struct GPUTexelCopyBufferLayout
    {
        GPUSize64 offset = 0;
        GPUSize32 bytes_per_row;
        GPUSize32 rows_per_image;
    };

    struct GPUTexelCopyBufferInfo : GPUTexelCopyBufferLayout
    {
        GPUBufferHandle buffer;
    };

    struct GPUTexelCopyTextureInfo
    {
        GPUTextureHandle      texture;
        GPUIntegerCoordinate  mip_level = 0;
        GPUOrigin3D           origin    = {};
        GPUTextureAspectFlags aspect    = GPUTextureAspect::ALL;
    };

    struct GPUBufferBindingLayout
    {
        GPUBufferBindingType type               = GPUBufferBindingType::UNIFORM;
        bool                 has_dynamic_offset = false;
        GPUSize64            min_binding_size   = 0;
    };

    struct GPUSamplerBindingLayout
    {
        GPUSamplerBindingType type = GPUSamplerBindingType::FILTERING;
    };

    struct GPUTextureBindingLayout
    {
        GPUTextureSampleType    sample_type    = GPUTextureSampleType::FLOAT;
        GPUTextureViewDimension view_dimension = GPUTextureViewDimension::x2D;
        bool                    multisampled   = false;
    };

    struct GPUStorageTextureBindingLayout
    {
        GPUStorageTextureAccess access = GPUStorageTextureAccess::WRITE_ONLY;
        GPUTextureFormat        format;
        GPUTextureViewDimension view_dimension = GPUTextureViewDimension::x2D;
    };

    struct GPUBVHBindingLayout
    {
        bool vertex_return = false;
    };

    struct GPUBVHSizes
    {
        uint bvh_size;
        uint build_size;
        uint update_size;
    };

    struct GPUBlasTriangleGeometrySizeDescriptor
    {
        GPUVertexFormat     vertex_format;
        GPUIndexFormat      index_format;
        uint                vertex_count;
        uint                index_count;
        GPUBVHGeometryFlags flags;
    };

    struct GPUBlasGeometrySizeDescriptor
    {
        GPUBlasType type = GPUBlasType::TRIANGLE;
        union
        {
            GPUBlasTriangleGeometrySizeDescriptor triangles;
        };

        // default trivial constructor / destructor
        GPUBlasGeometrySizeDescriptor() {}
        ~GPUBlasGeometrySizeDescriptor() {}
    };

    // NOTE: index is optional
    struct GPUBlasTriangleGeometry
    {
        GPUBlasTriangleGeometrySizeDescriptor size;
        GPUBufferHandle                       vertex_buffer;
        GPUBufferHandle                       index_buffer;
        GPUBufferHandle                       transform_buffer;
        uint                                  first_vertex;
        uint                                  first_index;
        GPUBufferAddress                      vertex_stride;
        GPUBufferAddress                      transform_buffer_offset;
    };

    struct GPUBlasGeometries
    {
        GPUBlasType type;
        union
        {
            GPUBlasTriangleGeometries triangles;
        };

        // default trivial constructor / destructor
        GPUBlasGeometries() {}
        ~GPUBlasGeometries() {}
    };

    struct GPUBlasBuildEntry
    {
        GPUBlasHandle     blas;
        GPUBlasGeometries geometries;
    };

    struct GPUTlasInstance
    {
        float         transform[4][3];
        uint32_t      custom_data;
        uint8_t       mask;
        GPUBlasHandle blas;
    };

    struct GPUTlasBuildEntry
    {
        GPUTlasHandle    tlas;
        GPUTlasInstances instances;
    };

    struct GPUBlendComponent
    {
        GPUBlendOperation operation  = GPUBlendOperation::ADD;
        GPUBlendFactor    src_factor = GPUBlendFactor::ONE;
        GPUBlendFactor    dst_factor = GPUBlendFactor::ZERO;
    };

    struct GPUBlendState
    {
        GPUBlendComponent color = {};
        GPUBlendComponent alpha = {};
    };

    // NOTE: shader_semantic is non-WebGPU standard, and only applicable to D3D12.
    // Users are expected to use the reflection or serialization/deserialization API
    // to automatically populate these.
    struct GPUVertexAttribute
    {
        GPUVertexFormat format;
        GPUSize64       offset;
        GPUIndex32      shader_location;
        CString         shader_semantic = nullptr;
    };

    struct GPUVertexBufferLayout
    {
        GPUSize64           array_stride;
        GPUVertexStepMode   step_mode = GPUVertexStepMode::VERTEX;
        GPUVertexAttributes attributes;
    };

    struct GPUStencilFaceState
    {
        GPUCompareFunction  compare       = GPUCompareFunction::ALWAYS;
        GPUStencilOperation fail_op       = GPUStencilOperation::KEEP;
        GPUStencilOperation depth_fail_op = GPUStencilOperation::KEEP;
        GPUStencilOperation pass_op       = GPUStencilOperation::KEEP;
    };

    struct GPUColorTargetState
    {
        GPUTextureFormat   format;
        GPUBlendState      blend;
        GPUColorWriteFlags write_mask   = GPUColorWrite::ALL;
        bool               blend_enable = false;
    };

    struct GPUProgrammableStage
    {
        GPUShaderModuleHandle                      module;
        CString                                    entry_point = "main";
        HashMap<CString, GPUPipelineConstantValue> constants   = {};
    };

    struct GPUBufferBinding
    {
        GPUBufferHandle buffer;
        GPUSize64       offset = 0;
        GPUSize64       size   = 0;
    };

    struct GPUBindGroupEntry
    {
        GPUIndex32             binding;
        GPUIndex32             index = 0; // NOTE: Non-WebGPU standard API
        GPUBindingResourceType type;
        union
        {
            GPUBufferBinding     buffer;
            GPUSamplerHandle     sampler;
            GPUTextureViewHandle texture;
        };

        // default trivial constructor / destructor
        GPUBindGroupEntry() {}
        ~GPUBindGroupEntry() {}
    };

    struct GPUBindGroupLayoutEntry
    {
        GPUBindingResourceType type;
        GPUBindingIndex        binding;
        GPUShaderStageFlags    visibility;
        GPUIndex32             count = 1; // NOTE: Non-WebGPU standard API
        union
        {
            GPUBufferBindingLayout         buffer;
            GPUSamplerBindingLayout        sampler;
            GPUTextureBindingLayout        texture;
            GPUStorageTextureBindingLayout storage_texture;
            GPUBVHBindingLayout            bvh;
        };

        // default trivial constructor / destructor
        GPUBindGroupLayoutEntry() {}
        ~GPUBindGroupLayoutEntry() {}
    };

    struct GPUVertexState : public GPUProgrammableStage
    {
        GPUVertexBufferLayouts buffers;
    };

    struct GPUFragmentState : public GPUProgrammableStage
    {
        GPUColorTargetStates targets;
    };

    struct GPUPrimitiveState
    {
        GPUPrimitiveTopology topology           = GPUPrimitiveTopology::TRIANGLE_LIST;
        GPUIndexFormat       strip_index_format = GPUIndexFormat::UINT32;
        GPUFrontFace         front_face         = GPUFrontFace::CCW;
        GPUCullMode          cull_mode          = GPUCullMode::NONE;

        // Requires "depth-clip-control" feature.
        bool unclipped_depth = false;
    };

    struct GPUDepthStencilState
    {
        GPUTextureFormat    format;
        bool                depth_write_enabled;
        GPUCompareFunction  depth_compare          = GPUCompareFunction::ALWAYS;
        GPUStencilFaceState stencil_front          = {};
        GPUStencilFaceState stencil_back           = {};
        GPUStencilValue     stencil_read_mask      = 0xFFFFFFFF;
        GPUStencilValue     stencil_write_mask     = 0xFFFFFFFF;
        GPUDepthBias        depth_bias             = 0;
        int                 depth_bias_constant    = 0;
        float               depth_bias_slope_scale = 0;
        float               depth_bias_clamp       = 0;
    };

    struct GPUMultisampleState
    {
        GPUSize32     count                     = 1;
        GPUSampleMask mask                      = 0xFFFFFFFF;
        bool          alpha_to_one_enabled      = false;
        bool          alpha_to_coverage_enabled = false;
    };

    struct GPURenderPassColorAttachment
    {
        GPUTextureViewHandle view;
        GPUIntegerCoordinate depth_slice;
        GPUTextureViewHandle resolve_target;
        GPUColor             clear_value;
        GPULoadOp            load_op;
        GPUStoreOp           store_op;
    };

    struct GPURenderPassDepthStencilAttachment
    {
        GPUTextureViewHandle view;
        float                depth_clear_value;
        GPULoadOp            depth_load_op;
        GPUStoreOp           depth_store_op;
        bool                 depth_read_only     = false;
        GPUStencilValue      stencil_clear_value = 0;
        GPULoadOp            stencil_load_op;
        GPUStoreOp           stencil_store_op;
        bool                 stencil_read_only = false;
    };

    struct GPUTextureSubresourceRange
    {
        GPUSize32 base_mip_level   = 0;
        GPUSize32 mip_level_count  = 1;
        GPUSize32 base_array_layer = 0;
        GPUSize32 array_layers     = 1;
    };

    // NOTE: Non-WebGPU standard API
    struct GPUMemoryBarrier
    {
        GPUBarrierSync   src_sync;
        GPUBarrierSync   dst_sync;
        GPUBarrierAccess src_access;
        GPUBarrierAccess dst_access;
    };

    // NOTE: Non-WebGPU standard API
    struct GPUBufferBarrier
    {
        GPUBarrierSync   src_sync;
        GPUBarrierSync   dst_sync;
        GPUBarrierAccess src_access;
        GPUBarrierAccess dst_access;
        GPUBufferHandle  buffer;
        GPUSize64        offset;
        GPUSize64        size;
    };

    // NOTE: Non-WebGPU standard API
    struct GPUTextureBarrier
    {
        GPUBarrierSync             src_sync;
        GPUBarrierSync             dst_sync;
        GPUBarrierAccess           src_access;
        GPUBarrierAccess           dst_access;
        GPUBarrierLayout           src_layout;
        GPUBarrierLayout           dst_layout;
        GPUTextureHandle           texture;
        GPUTextureSubresourceRange subresources;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_UTILS_H
