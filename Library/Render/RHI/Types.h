#ifndef LYRA_LIBRARY_RENDER_RHI_TYPES_H
#define LYRA_LIBRARY_RENDER_RHI_TYPES_H

// reference: https://gpuweb.github.io/gpuweb/#

#include <Render/RHI/Enums.h>
#include <Render/RHI/Utils.h>
#include <Render/RHI/Descs.h>
#include <Render/RHI/Mixin.h>
#include <Render/RHI/Error.h>

namespace lyra::rhi
{
    struct GPUObjectBase
    {
        CString label = "";

        GPUDeviceHandle device;
    };

    struct GPUSampler : public GPUObjectBase
    {
        GPUSamplerHandle handle;

        void destroy();
    };

    struct GPUShaderModule : public GPUObjectBase
    {
        GPUShaderModuleHandle handle;

        void destroy();
    };

    struct GPUTextureView : public GPUObjectBase
    {
        GPUTextureViewHandle handle;

        void destroy();
    };

    struct GPUTexture : public GPUObjectBase
    {
        GPUTextureHandle handle;

        GPUIntegerCoordinateOut width;
        GPUIntegerCoordinateOut height;
        GPUIntegerCoordinate    depth_or_array_layers;
        GPUSize32Out            sample_count;
        GPUTextureDimension     dimension;
        GPUTextureFormat        format;
        GPUFlagsConstant        usage;

        auto create_view() -> GPUTextureView;

        auto create_view(GPUTextureViewDescriptor descriptor) -> GPUTextureView;

        auto destroy() -> void;
    };

    struct GPUBuffer : public GPUObjectBase
    {
        GPUBufferHandle handle;

        GPUSize64Out   size      = 0;
        GPUBufferUsage usage     = static_cast<GPUBufferUsage>(0);
        GPUMapState    map_state = GPUMapState::UNMAPPED;

        auto get_mapped_range(GPUSize64 offset = 0, GPUSize64 size = 0) -> MappedBufferRange;

        auto map(GPUMapMode mode, GPUSize64 offset, GPUSize64 size) -> void;

        auto unmap() -> void;

        auto destroy() -> void;
    };

    struct GPUQuerySet : public GPUObjectBase
    {
        GPUQuerySetHandle handle;

        GPUQueryType type;
        GPUSize32Out count;

        auto destroy() -> void;
    };

    struct GPUTlas : public GPUObjectBase
    {
        GPUTlasHandle handle;

        auto destroy() -> void;
    };

    struct GPUBlas : public GPUObjectBase
    {
        GPUBlasHandle handle;

        auto destroy() -> void;
    };

    struct GPUBindGroup : public GPUObjectBase
    {
        GPUBindGroupHandle handle;

        // NOTE: no manual deletion of GPUBindGroup,
        // because these are automatically recycled by GC.
    };

    struct GPUBindGroupLayout : public GPUObjectBase
    {
        GPUBindGroupHandle handle;

        // NOTE: no manual deletion of GPUBindGroupLayout,
        // because it is cheap and shared and usually cause bad situations if managed manually.
    };

    struct GPUPipelineLayout : public GPUObjectBase
    {
        GPUPipelineLayoutHandle handle;

        // NOTE: no manual deletion of GPUPipelineLayout,
        // because it is cheap and shared and usually cause bad situations if managed manually.
    };

    struct GPURenderPipeline : public GPUObjectBase
    {
        GPURenderPipelineHandle handle;

        // NOTE: no manual deletion of GPURenderPipeline,
        // because it is shared and usually cause bad situations if managed manually.
    };

    struct GPUComputePipeline : public GPUObjectBase
    {
        GPUComputePipelineHandle handle;

        // NOTE: no manual deletion of GPURenderPipeline,
        // because it is shared and usually cause bad situations if managed manually.
    };

    struct GPURayTracingPipeline : public GPUObjectBase
    {
        GPURayTracingPipelineHandle handle;

        // NOTE: no manual deletion of GPURenderPipeline,
        // because it is shared and usually cause bad situations if managed manually.
    };

    struct GPUComputePassEncoder : public GPUObjectBase,
                                   public GPUCommandsMixin,
                                   public GPUDebugCommandsMixin,
                                   public GPUBindingCommandsMixin
    {
        void set_pipeline(GPUComputePipeline pipeline);

        void dispatch_workgroups(
            GPUSize32 workgroup_count_x,
            GPUSize32 workgroup_count_y = 1,
            GPUSize32 workgroup_count_z = 1);

        void dispatch_Workgroups_indirect(
            GPUBufferHandle indirect_buffer,
            GPUSize64       indirect_offset);

        void end();
    };

    struct GPURenderPassEncoder : public GPUObjectBase,
                                  public GPUCommandsMixin,
                                  public GPUDebugCommandsMixin,
                                  public GPUBindingCommandsMixin,
                                  public GPURenderCommandsMixin
    {
        void set_viewport(
            float x, float y,
            float width, float height,
            float min_depth, float max_depth);

        void set_scissor_rect(
            GPUIntegerCoordinate x,
            GPUIntegerCoordinate y,
            GPUIntegerCoordinate width,
            GPUIntegerCoordinate height);

        void set_blend_constant(GPUColor color);

        void set_stencil_reference(GPUStencilValue reference);

        void begin_occlusion_query(GPUSize32 queryIndex);

        void end_occlusion_query();

        void execute_bundles(const Vector<GPURenderBundleHandle>& bundles);

        void end();
    };

    struct GPURenderBundleEncoder : public GPUObjectBase,
                                    public GPUCommandsMixin,
                                    public GPUDebugCommandsMixin,
                                    public GPUBindingCommandsMixin,
                                    public GPURenderCommandsMixin
    {
        GPURenderBundleHandle finish(const GPURenderBundleDescriptor& descriptor = {});
    };

    struct GPURenderBundle : public GPUObjectBase
    {
        GPURenderBundleHandle handle;
    };

    struct GPUCommandBuffer : public GPUObjectBase
    {
        GPUCommandBufferHandle handle;
    };

    struct GPUCommandEncoder : public GPUObjectBase
    {
        GPURenderPassEncoder begin_render_pass(const GPURenderPassDescriptor& descriptor);

        GPUComputePassEncoder begin_compute_pass(const GPUComputePassDescriptor& descriptor = {});

        void copy_buffer_to_buffer(
            GPUBufferHandle source,
            GPUBufferHandle destination,
            GPUSize64       size);

        void copy_buffer_to_buffer(
            GPUBufferHandle source,
            GPUSize64       source_offset,
            GPUBufferHandle destination,
            GPUSize64       destination_offset,
            GPUSize64       size);

        void copy_buffer_to_texture(
            GPUTexelCopyBufferInfo  source,
            GPUTexelCopyTextureInfo destination,
            GPUExtent3D             copy_size);

        void copy_texture_to_buffer(
            GPUTexelCopyTextureInfo source,
            GPUTexelCopyBufferInfo  destination,
            GPUExtent3D             copy_size);

        void copy_texture_to_texture(
            GPUTexelCopyTextureInfo source,
            GPUTexelCopyTextureInfo destination,
            GPUExtent3D             copy_size);

        void clear_buffer(
            GPUBufferHandle buffer,
            GPUSize64       offset = 0,
            GPUSize64       size   = 0);

        void resolve_query_set(
            GPUQuerySet query_set,
            GPUSize32   first_query,
            GPUSize32   query_count,
            GPUBuffer   destination,
            GPUSize64   destination_offset);

        GPUCommandBuffer finish(const GPUCommandBufferDescriptor& descriptor = {});
    };

    struct GPUQueue : public GPUObjectBase
    {
        GPUQueueHandle handle;

        auto submit(const Vector<GPUCommandBufferHandle>& command_buffers) -> GPUSubmissionIndex;

        template <typename F>
        void on_submitted_work_done(F&& f);

        void write_buffer(
            GPUBufferHandle buffer,
            GPUSize64       buffer_offset,
            BufferSource    data,
            GPUSize64       data_offset = 0,
            GPUSize64       size        = 0);

        void write_texture(
            GPUTexelCopyTextureInfo  destination,
            BufferSource             data,
            GPUTexelCopyBufferLayout dataLayout,
            GPUExtent3D              size);
    };

    struct GPUSurfaceTexture : public GPUObjectBase
    {
        GPUTextureHandle handle;
        bool             suboptimal;

        void present();
    };

    struct GPUDevice : public GPUObjectBase
    {
        GPUDeviceHandle handle;

        GPUAdapterInfo       adapter_info   = {};
        GPUSupportedFeatures features       = {};
        GPUQueue             compute_queue  = {};
        GPUQueue             graphics_queue = {};
        GPUQueue             transfer_queue = {};

        auto create_buffer(const GPUBufferDescriptor& descriptor) -> GPUBuffer;

        auto create_texture(const GPUTextureDescriptor& descriptor) -> GPUTexture;

        auto create_sampler(const GPUSamplerDescriptor& descriptor) -> GPUSampler;

        auto create_shader_module(const GPUShaderModuleDescriptor& descriptor) -> GPUShaderModule;

        auto create_query_set(const GPUQuerySetDescriptor& descriptor) -> GPUQuerySet;

        auto create_bind_group(const GPUBindGroupDescriptor& descriptor) -> GPUBindGroup;

        auto create_bind_group_layout(const GPUBindGroupLayoutDescriptor& descriptor) -> GPUBindGroupLayout;

        auto create_pipeline_layout(const GPUPipelineLayoutDescriptor& descriptor) -> GPUPipelineLayout;

        auto create_render_pipeline(const GPURenderPipelineDescriptor& descriptor) -> GPURenderPipeline;

        auto create_compute_pipeline(const GPUComputePipelineDescriptor& descriptor) -> GPUComputePipeline;

        auto create_raytracing_pipeline(const GPURayTracingPipelineDescriptor& descriptor) -> GPURayTracingPipeline;

        auto create_command_encoder(const GPUCommandEncoderDescriptor& descriptor) -> GPUCommandEncoder;

        auto create_render_bundle_encoder(const GPURenderBundleEncoderDescriptor& descriptor) -> GPURenderBundleEncoder;

        auto push_error_scope(GPUErrorFilter filter) -> void;

        auto pop_error_scope() -> void;

        auto destroy() -> void;
    };

    struct GPUAdapter : public GPUObjectBase
    {
        GPUAdapterInfo       info     = {};
        GPUSupportedFeatures features = {};
        GPUSupportedLimits   limits   = {};

        auto request_device(const GPUDeviceDescriptor& descriptor) -> GPUDevice;
    };

    struct GPUSurface : public GPUObjectBase
    {
        GPUSurfaceHandle handle;

        auto destroy() -> void;

        auto get_surface_capabilities() -> GPUSurfaceCapabilities;

        auto get_current_texture() -> GPUTexture;
    };

    struct GPU
    {
        GPUBackend   backend;
        GPUFlags     flags  = 0;
        WindowHandle window = {};

        explicit GPU(const GPUDescriptor& descriptor);

        auto destroy() -> void;

        auto request_adapter(const GPUAdapterDescriptor& descriptor = {}) -> GPUAdapter;

        auto request_surface(const GPUDevice& device, const GPUSurfaceDescriptor& descriptor = {}) -> GPUSurface;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_TYPES_H
