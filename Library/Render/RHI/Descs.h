#ifndef LYRA_LIBRARY_RENDER_RHI_DESCS_H
#define LYRA_LIBRARY_RENDER_RHI_DESCS_H

#include <Window/Window.hpp>

#include <Render/RHI/Enums.h>
#include <Render/RHI/Utils.h>

namespace lyra::rhi
{
    using namespace lyra::wsi;

    struct GPUObjectDescriptorBase
    {
        CString label = "";
    };

    struct RHIDescriptor : public GPUObjectDescriptorBase
    {
        RHIFlags     flags = 0;
        RHIBackend   backend;
        WindowHandle window = {};
    };

    struct GPUAdapterDescriptor : public GPUObjectDescriptorBase
    {
        // nothing here for now
    };

    struct GPUDeviceDescriptor : public GPUObjectDescriptorBase
    {
        Vector<GPUFeatureName> required_features = {};
    };

    struct GPUSurfaceDescriptor : public GPUObjectDescriptorBase
    {
        WindowHandle window = {};

        GPUCompositeAlphaMode alpha_mode      = GPUCompositeAlphaMode::Opaque;
        GPUPresentMode        present_mode    = GPUPresentMode::Fifo;
        GPUColorSpace         color_space     = GPUColorSpace::SRGB;
        uint                  frames_inflight = 3;
    };

    struct GPUQueueDescriptor : public GPUObjectDescriptorBase
    {
        // nothing here for now
    };

    struct GPUBufferDescriptor : public GPUObjectDescriptorBase
    {
        GPUSize64           size               = 0;
        GPUBufferUsageFlags usage              = 0;
        bool                mapped_at_creation = false;
    };

    struct GPUSamplerDescriptor : public GPUObjectDescriptorBase
    {
        GPUAddressMode      address_mode_u = GPUAddressMode::CLAMP_TO_EDGE;
        GPUAddressMode      address_mode_v = GPUAddressMode::CLAMP_TO_EDGE;
        GPUAddressMode      address_mode_w = GPUAddressMode::CLAMP_TO_EDGE;
        GPUFilterMode       mag_filter     = GPUFilterMode::NEAREST;
        GPUFilterMode       min_filter     = GPUFilterMode::NEAREST;
        GPUMipmapFilterMode mipmap_filter  = GPUMipmapFilterMode::NEAREST;
        float               lod_min_clamp  = 0;
        float               lod_max_clamp  = 32;
        GPUCompareFunction  compare        = GPUCompareFunction::GREATER;
        unsigned short      max_anisotropy = 1;
        bool                compare_enable = false;
    };

    struct GPUTextureDescriptor : public GPUObjectDescriptorBase
    {
        GPUExtent3D          size;
        GPUIntegerCoordinate mip_level_count = 1;
        GPUIntegerCoordinate array_layers    = 1;
        GPUSize32            sample_count    = 1;
        GPUTextureDimension  dimension       = GPUTextureDimension::x2D;
        GPUTextureFormat     format          = GPUTextureFormat::RGBA8UNORM;
        GPUTextureUsageFlags usage           = 0;
    };

    struct GPUTextureViewDescriptor : public GPUObjectDescriptorBase
    {
        GPUTextureFormat        format;
        GPUTextureViewDimension dimension;
        GPUTextureUsageFlags    usage             = 0;
        GPUTextureAspect        aspect            = GPUTextureAspect::ALL;
        GPUIntegerCoordinate    base_mip_level    = 0;
        GPUIntegerCoordinate    mip_level_count   = 1;
        GPUIntegerCoordinate    base_array_layer  = 0;
        GPUIntegerCoordinate    array_layer_count = 1;
    };

    struct GPUShaderModuleDescriptor : public GPUObjectDescriptorBase
    {
        uint8_t* data = nullptr;
        uint     size = 0;
    };

    struct GPUQuerySetDescriptor : public GPUObjectDescriptorBase
    {
        GPUQueryType type;
        GPUSize32    count;
    };

    struct GPUTlasDescriptor : public GPUObjectDescriptorBase
    {
        // nothing here for now
    };

    struct GPUBlasDescriptor : public GPUObjectDescriptorBase
    {
        // nothing here for now
    };

    struct GPUBindGroupDescriptor : public GPUObjectDescriptorBase
    {
        GPUBindGroupLayoutHandle  layout;
        Vector<GPUBindGroupEntry> entries;
    };

    struct GPUBindGroupLayoutDescriptor : public GPUObjectDescriptorBase
    {
        Vector<GPUBindGroupLayoutEntry> entries = {};
    };

    struct GPUPipelineLayoutDescriptor : public GPUObjectDescriptorBase
    {
        Vector<GPUBindGroupLayoutHandle> bind_group_layouts;
    };

    struct GPUComputePipelineDescriptor
    {
        GPUPipelineLayoutHandle layout;
        GPUProgrammableStage    compute;
    };

    struct GPURenderPipelineDescriptor
    {
        GPUPipelineLayoutHandle layout;
        GPUVertexState          vertex        = {};
        GPUPrimitiveState       primitive     = {};
        GPUDepthStencilState    depth_stencil = {};
        GPUMultisampleState     multisample   = {};
        GPUFragmentState        fragment      = {};
    };

    struct GPURayTracingPipelineDescriptor
    {
        // more stuff in here
        GPUPipelineLayoutHandle layout;
    };

    struct GPURenderPassLayout : public GPUObjectDescriptorBase
    {
        Vector<GPUTextureFormat> color_formats;
        GPUTextureFormat         depth_stencil_format;
        GPUSize32                sample_count = 1;
    };

    struct GPUCommandBundleDescriptor : public GPUObjectDescriptorBase
    {
        // nothing here for now
    };

    struct GPUComputePassDescriptor : public GPUObjectDescriptorBase
    {
        GPUComputePassTimestampWrites timestampWrites;
    };

    struct GPURenderPassDescriptor : public GPUObjectDescriptorBase
    {
        Vector<GPURenderPassColorAttachment> color_attachments;
        GPURenderPassDepthStencilAttachment  depth_stencil_attachment;
        GPUQuerySetHandle                    occlusion_query_set;
        GPURenderPassTimestampWrites         timestamp_writes;
        GPUSize64                            max_draw_count = 50000000;
    };

    struct GPUCommandBufferDescriptor : public GPUObjectDescriptorBase
    {
        // nothing here for now
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_DESCS_H
