#ifndef LYRA_LIBRARY_RENDER_RHI_TYPES_H
#define LYRA_LIBRARY_RENDER_RHI_TYPES_H

// reference: https://gpuweb.github.io/gpuweb/#

#include <Common/Pointer.h>
#include <Render/RHI/Enums.h>
#include <Render/RHI/Utils.h>
#include <Render/RHI/Descs.h>
#include <Render/RHI/Error.h>

namespace lyra::rhi
{
    struct GPUObjectBase
    {
        CString label = "";
    };

    struct GPUFence : public GPUObjectBase
    {
        GPUFenceHandle handle;

        void destroy();

        operator GPUFenceHandle() const { return handle; }
    };

    struct GPUSampler : public GPUObjectBase
    {
        GPUSamplerHandle handle;

        void destroy();

        operator GPUSamplerHandle() const { return handle; }
    };

    struct GPUShaderModule : public GPUObjectBase
    {
        GPUShaderModuleHandle handle;

        void destroy();

        operator GPUShaderModuleHandle() const { return handle; }
    };

    struct GPUTextureView : public GPUObjectBase
    {
        GPUTextureViewHandle handle;

        void destroy();

        operator GPUTextureViewHandle() const { return handle; }
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

        operator GPUTextureHandle() const { return handle; }
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

        operator GPUBufferHandle() const { return handle; }
    };

    struct GPUQuerySet : public GPUObjectBase
    {
        GPUQuerySetHandle handle;

        GPUQueryType type;
        GPUSize32Out count;

        auto destroy() -> void;

        operator GPUQuerySetHandle() const { return handle; }
    };

    struct GPUTlas : public GPUObjectBase
    {
        GPUTlasHandle handle;

        auto destroy() -> void;

        operator GPUTlasHandle() const { return handle; }
    };

    struct GPUBlas : public GPUObjectBase
    {
        GPUBlasHandle handle;

        auto destroy() -> void;

        operator GPUBlasHandle() const { return handle; }
    };

    struct GPUBindGroup : public GPUObjectBase
    {
        GPUBindGroupHandle handle;

        // NOTE: no manual deletion of GPUBindGroup,
        // because these are automatically recycled by GC.

        operator GPUBindGroupHandle() const { return handle; }
    };

    struct GPUBindGroupLayout : public GPUObjectBase
    {
        GPUBindGroupLayoutHandle handle;

        void destroy();

        operator GPUBindGroupLayoutHandle() const { return handle; }
    };

    struct GPUPipelineLayout : public GPUObjectBase
    {
        GPUPipelineLayoutHandle handle;

        void destroy();

        operator GPUPipelineLayoutHandle() const { return handle; }
    };

    struct GPURenderPipeline : public GPUObjectBase
    {
        GPURenderPipelineHandle handle;

        void destroy();

        operator GPURenderPipelineHandle() const { return handle; }
    };

    struct GPUComputePipeline : public GPUObjectBase
    {
        GPUComputePipelineHandle handle;

        void destroy();

        operator GPUComputePipelineHandle() const { return handle; }
    };

    struct GPURayTracingPipeline : public GPUObjectBase
    {
        GPURayTracingPipelineHandle handle;

        void destroy();

        operator GPURayTracingPipelineHandle() const { return handle; }
    };

    struct GPUCommandEncoder : public GPUObjectBase
    {
        GPUQueueType type = GPUQueueType::DEFAULT;

        GPUCommandEncoderHandle handle;

        void insert_debug_marker(CString marker_label);

        void push_debug_group(CString group_label);

        void pop_debug_group();

        void set_pipeline(GPURenderPipeline pipeline);

        void set_pipeline(GPUComputePipeline pipeline);

        void set_pipeline(GPURayTracingPipeline pipeline);

        void set_bind_group(GPUIndex32 index, GPUBindGroup bind_group, const Vector<GPUBufferDynamicOffset>& dynamic_offsets = {});

        void dispatch_workgroups(GPUSize32 x, GPUSize32 y = 1, GPUSize32 z = 1);

        void dispatch_Workgroups_indirect(const GPUBuffer& indirect_buffer, GPUSize64 indirect_offset);

        void set_index_buffer(const GPUBuffer& buffer, GPUIndexFormat index_format, GPUSize64 offset = 0, GPUSize64 size = 0);

        void set_vertex_buffer(GPUIndex32 slot, const GPUBuffer& buffer, GPUSize64 offset = 0, GPUSize64 size = 0);

        void draw(GPUSize32 vertex_count, GPUSize32 instance_count = 1, GPUSize32 first_vertex = 0, GPUSize32 first_instance = 0);

        void draw_indexed(GPUSize32 index_count, GPUSize32 instance_count = 1, GPUSize32 first_index = 0, GPUSignedOffset32 base_vertex = 0, GPUSize32 first_instance = 0);

        void draw_indirect(const GPUBuffer& buffer, GPUSize64 indirect_offset);

        void draw_indexed_indirect(const GPUBuffer& buffer, GPUSize64 indirect_offset);

        void begin_render_pass(const GPURenderPassDescriptor& descriptor);

        void begin_compute_pass(const GPUComputePassDescriptor& descriptor = {});

        void copy_buffer_to_buffer(const GPUBuffer& source, const GPUBuffer& destination, GPUSize64 size);

        void copy_buffer_to_buffer(const GPUBuffer& source, GPUSize64 source_offset, const GPUBuffer& destination, GPUSize64 destination_offset, GPUSize64 size);

        void copy_buffer_to_texture(GPUTexelCopyBufferInfo source, GPUTexelCopyTextureInfo destination, GPUExtent3D copy_size);

        void copy_texture_to_buffer(GPUTexelCopyTextureInfo source, GPUTexelCopyBufferInfo destination, GPUExtent3D copy_size);

        void copy_texture_to_texture(GPUTexelCopyTextureInfo source, GPUTexelCopyTextureInfo destination, GPUExtent3D copy_size);

        void clear_buffer(const GPUBuffer& buffer, GPUSize64 offset = 0, GPUSize64 size = 0);

        void resolve_query_set(GPUQuerySet query_set, GPUSize32 first_query, GPUSize32 query_count, const GPUBuffer& destination, GPUSize64 destination_offset);

        void set_viewport(float x, float y, float w, float h, float min_depth, float max_depth);

        void set_scissor_rect(GPUIntegerCoordinate x, GPUIntegerCoordinate y, GPUIntegerCoordinate w, GPUIntegerCoordinate h);

        void set_blend_constant(GPUColor color);

        void set_stencil_reference(GPUStencilValue reference);

        void begin_occlusion_query(GPUSize32 queryIndex);

        void end_occlusion_query();

        void buffer_barrier(const Vector<BufferBarrier>& barriers);

        void texture_barrier(const Vector<TextureBarrier>& barriers);

        void finish();
    };

    struct GPUCommandBundle : public GPUCommandEncoder
    {
        operator GPUCommandEncoderHandle() const { return handle; }
    };

    struct GPUCommandBuffer : public GPUCommandEncoder
    {
        operator GPUCommandEncoderHandle() const { return handle; }

        void execute_bundles(const Vector<GPUCommandBundle>& bundles);
    };

    struct GPUQueue : public GPUObjectBase
    {
        GPUQueueHandle handle;

        void submit(const Vector<GPUCommandBuffer>& command_buffers);

        void write_buffer(const GPUBuffer& buffer, GPUSize64 buffer_offset, BufferSource data, GPUSize64 data_offset = 0, GPUSize64 size = 0);

        void write_texture(GPUTexelCopyTextureInfo destination, BufferSource data, GPUTexelCopyBufferLayout dataLayout, GPUExtent3D size);

        operator GPUQueueHandle() const { return handle; }
    };

    struct GPUSurfaceTexture : public GPUObjectBase
    {
        GPUTextureHandle handle;
        GPUFenceHandle   ready;
        bool             suboptimal;

        void present();

        operator GPUTextureHandle() const { return handle; }
    };

    struct GPUDevice : public GPUObjectBase
    {
        GPUAdapterInfo       adapter_info   = {};
        GPUSupportedFeatures features       = {};
        GPUQueue             default_queue  = {};
        GPUQueue             compute_queue  = {};
        GPUQueue             transfer_queue = {};

        auto create_fence() -> GPUFence;

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

        auto create_command_buffer(const GPUCommandBufferDescriptor& descriptor, GPUQueueType type = GPUQueueType::DEFAULT) -> GPUCommandBuffer;

        auto create_command_bundle(const GPUCommandBundleDescriptor& descriptor, GPUQueueType type = GPUQueueType::DEFAULT) -> GPUCommandBundle;

        auto push_error_scope(GPUErrorFilter filter) -> void;

        auto pop_error_scope() -> void;

        auto wait() -> void;

        auto wait(GPUFence fence) -> void;

        auto destroy() -> void;
    };

    struct GPUSurface : public GPUObjectBase
    {
        GPUSurfaceCapabilities capabilities;

        auto destroy() -> void;

        auto get_current_texture() -> GPUTexture;
    };

    struct GPUAdapter : public GPUObjectBase
    {
        GPUAdapterInfo       info     = {};
        GPUSupportedFeatures features = {};
        GPUSupportedLimits   limits   = {};

        auto request_device(const GPUDeviceDescriptor& descriptor) -> GPUDevice;
    };

    struct RHI
    {
        static GPUDevice  DEVICE;
        static GPUSurface SURFACE;

        RHIFlags     flags = 0;
        RHIBackend   backend;
        WindowHandle window = {};

        static GPUDevice& get_current_device() { return DEVICE; }

        static GPUSurface& get_current_surface() { return SURFACE; }

        static auto init(const RHIDescriptor& descriptor) -> OwnedResource<RHI>;

        auto destroy() -> void;

        auto request_adapter(const GPUAdapterDescriptor& descriptor = {}) -> GPUAdapter;

        auto request_surface(const GPUSurfaceDescriptor& descriptor = {}) -> GPUSurface;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_TYPES_H
