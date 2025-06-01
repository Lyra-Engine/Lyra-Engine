#ifndef LYRA_LIBRARY_RENDER_RHI_UTILS_H
#define LYRA_LIBRARY_RENDER_RHI_UTILS_H

#include <Common/String.h>
#include <Common/Handle.h>
#include <Common/BitFlags.h>
#include <Common/Container.h>

#include <Render/RHI/Enums.h>

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
    using RHIFlags                 = BitFlags<RHIFlag>;
    using GPUShaderStageFlags      = BitFlags<GPUShaderStage>;
    using GPUBufferUsageFlags      = BitFlags<GPUBufferUsage>;
    using GPUTextureUsageFlags     = BitFlags<GPUTextureUsage>;
    using GPUColorWriteFlags       = BitFlags<GPUColorWrite>;
    using GPUBarrierSyncFlags      = BitFlags<GPUBarrierSync>;
    using GPUBarrierAccessFlags    = BitFlags<GPUBarrierAccess>;

    // typed GPU handle
    using GPUFenceHandle              = Handle<GPUObjectType, GPUObjectType::FENCE>;
    using GPUQueueHandle              = Handle<GPUObjectType, GPUObjectType::QUEUE>;
    using GPUCommandEncoderHandle     = Handle<GPUObjectType, GPUObjectType::COMMAND_ENCODER>;
    using GPUBufferHandle             = Handle<GPUObjectType, GPUObjectType::BUFFER>;
    using GPUSamplerHandle            = Handle<GPUObjectType, GPUObjectType::SAMPLER>;
    using GPUTextureHandle            = Handle<GPUObjectType, GPUObjectType::TEXTURE>;
    using GPUTextureViewHandle        = Handle<GPUObjectType, GPUObjectType::TEXTURE_VIEW>;
    using GPUShaderModuleHandle       = Handle<GPUObjectType, GPUObjectType::SHADER_MODULE>;
    using GPUQuerySetHandle           = Handle<GPUObjectType, GPUObjectType::QUERY_SET>;
    using GPUTlasHandle               = Handle<GPUObjectType, GPUObjectType::TLAS>;
    using GPUBlasHandle               = Handle<GPUObjectType, GPUObjectType::BLAS>;
    using GPUBindGroupHandle          = Handle<GPUObjectType, GPUObjectType::BIND_GROUP>;
    using GPUBindGroupLayoutHandle    = Handle<GPUObjectType, GPUObjectType::BIND_GROUP_LAYOUT>;
    using GPUPipelineLayoutHandle     = Handle<GPUObjectType, GPUObjectType::PIPELINE_LAYOUT>;
    using GPURenderPipelineHandle     = Handle<GPUObjectType, GPUObjectType::RENDER_PIPELINE>;
    using GPUComputePipelineHandle    = Handle<GPUObjectType, GPUObjectType::COMPUTE_PIPELINE>;
    using GPURayTracingPipelineHandle = Handle<GPUObjectType, GPUObjectType::RAYTRACING_PIPELINE>;

    struct MappedBufferRange
    {
        BufferSource data;
        size_t       size;
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

    struct GPUAdapterInfo
    {
        String architecture      = "";
        String descrition        = "";
        String device            = "";
        String vendor            = "";
        uint   subgroup_max_size = 0;
        uint   subgroup_min_size = 0;
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
        GPUTextureHandle     texture;
        GPUIntegerCoordinate mip_level = 0;
        GPUOrigin3D          origin    = {};
        GPUTextureAspect     aspect    = GPUTextureAspect::ALL;
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

    struct GPUVertexAttribute
    {
        GPUVertexFormat format;
        GPUSize64       offset;
        GPUIndex32      shader_location;
    };

    struct GPUVertexBufferLayout
    {
        GPUSize64                  array_stride;
        GPUVertexStepMode          step_mode = GPUVertexStepMode::VERTEX;
        Vector<GPUVertexAttribute> attributes;
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
        CString                                    entry_point = "";
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
        GPUBindingResourceType type;
        union
        {
            GPUBufferBinding     buffer;
            GPUSamplerHandle     sampler;
            GPUTextureViewHandle texture;
        };
    };

    struct GPUBindGroupLayoutEntry
    {
        GPUIndex32             binding;
        GPUIndex32             count = 1;
        GPUShaderStageFlags    visibility;
        GPUBindingResourceType type;
        union
        {
            GPUBufferBindingLayout         buffer;
            GPUSamplerBindingLayout        sampler;
            GPUTextureBindingLayout        texture;
            GPUStorageTextureBindingLayout storage_texture;
        };
    };

    struct GPUVertexState : public GPUProgrammableStage
    {
        Vector<GPUVertexBufferLayout> buffers = {};
    };

    struct GPUFragmentState : public GPUProgrammableStage
    {
        Vector<GPUColorTargetState> targets = {};
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
        GPUCompareFunction  depth_compare;
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

    struct GPURenderPassTimestampWrites
    {
        GPUQuerySetHandle query_set;
        GPUSize32         beginning_of_pass_write_index;
        GPUSize32         end_of_pass_write_index;
    };

    struct GPUComputePassTimestampWrites
    {
        GPUQuerySetHandle query_set;
        GPUSize32         beginning_of_pass_write_index;
        GPUSize32         end_of_pass_write_index;
    };

    struct GPUTextureSubresourceRange
    {
        GPUSize32 base_mip_level   = 0;
        GPUSize32 level_count      = 0;
        GPUSize32 base_array_layer = 0;
        GPUSize32 layer_count      = 0;
    };

    struct BufferBarrier
    {
        GPUBarrierSync   src_sync;
        GPUBarrierSync   dst_sync;
        GPUBarrierAccess src_access;
        GPUBarrierAccess dst_access;
        GPUBufferHandle  buffer;
        GPUSize64        offset;
        GPUSize64        size;
    };

    struct TextureBarrier
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
