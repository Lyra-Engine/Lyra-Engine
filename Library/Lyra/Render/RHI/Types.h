#ifndef LYRA_LIBRARY_RENDER_RHI_TYPES_H
#define LYRA_LIBRARY_RENDER_RHI_TYPES_H

// reference: https://gpuweb.github.io/gpuweb/#

#include <Lyra/Common/Pointer.h>
#include <Lyra/Render/RHI/Enums.h>
#include <Lyra/Render/RHI/Utils.h>
#include <Lyra/Render/RHI/Descs.h>
#include <Lyra/Render/RHI/Error.h>

namespace lyra::rhi
{
    struct RenderAPI;

    struct GPUObjectBase
    {
        CString label = "";
    };

    struct GPUFence : public GPUObjectBase
    {
        GPUFenceHandle handle;

        // implicit conversion
        GPUFence() : handle() {}
        GPUFence(GPUFenceHandle handle) : handle(handle) {}

        void wait() const;

        void destroy();

        operator GPUFenceHandle() const { return handle; }
    };

    struct GPUSampler : public GPUObjectBase
    {
        GPUSamplerHandle handle;

        // implicit conversion
        GPUSampler() : handle() {}
        GPUSampler(GPUSamplerHandle handle) : handle(handle) {}

        void destroy();

        operator GPUSamplerHandle() const { return handle; }
    };

    struct GPUShaderModule : public GPUObjectBase
    {
        GPUShaderModuleHandle handle;

        // implicit conversion
        GPUShaderModule() : handle() {}
        GPUShaderModule(GPUShaderModuleHandle handle) : handle(handle) {}

        void destroy();

        operator GPUShaderModuleHandle() const { return handle; }
    };

    struct GPUTextureView : public GPUObjectBase
    {
        GPUTextureViewHandle handle;

        // implicit conversion
        GPUTextureView() : handle() {}
        GPUTextureView(GPUTextureViewHandle handle) : handle(handle) {}

        void destroy();

        operator GPUTextureViewHandle() const { return handle; }
    };

    struct GPUTexture : public GPUObjectBase
    {
        GPUTextureHandle handle;

        GPUIntegerCoordinateOut width;
        GPUIntegerCoordinateOut height;
        GPUIntegerCoordinate    depth;
        GPUIntegerCoordinate    array_layers;
        GPUIntegerCoordinate    mip_level_count;
        GPUSize32Out            sample_count;
        GPUTextureDimension     dimension;
        GPUTextureFormat        format;
        GPUTextureUsageFlags    usage;

        auto create_view() -> GPUTextureView;

        auto create_view(GPUTextureViewDescriptor descriptor) -> GPUTextureView;

        void destroy();

        operator GPUTextureHandle() const { return handle; }
    };

    struct GPUBuffer : public GPUObjectBase
    {
        GPUBufferHandle handle;

        GPUSize64Out        size      = 0;
        GPUBufferUsageFlags usage     = 0;
        GPUMapState         map_state = GPUMapState::UNMAPPED;

        template <typename T>
        auto get_mapped_range() const -> TypedBufferRange<T>
        {
            auto range  = get_mapped_range();
            auto typed  = TypedBufferRange<T>{};
            typed.data  = reinterpret_cast<T*>(range.data);
            typed.count = range.size / sizeof(T);
            return typed;
        }

        auto get_mapped_range() const -> MappedBufferRange;

        void map(GPUMapMode mode, GPUSize64 offset = 0, GPUSize64 size = 0);

        void unmap();

        void destroy();

        operator GPUBufferHandle() const { return handle; }
    };

    struct GPUQuerySet : public GPUObjectBase
    {
        GPUQuerySetHandle handle;

        GPUQueryType type;
        GPUSize32    count;

        auto destroy() -> void;

        operator GPUQuerySetHandle() const { return handle; }
    };

    struct GPUTlas : public GPUObjectBase
    {
        GPUTlasHandle handle;

        // implicit conversion
        GPUTlas() : handle() {}
        GPUTlas(GPUTlasHandle handle) : handle(handle) {}

        auto destroy() -> void;

        operator GPUTlasHandle() const { return handle; }
    };

    struct GPUBlas : public GPUObjectBase
    {
        GPUBlasHandle handle;

        // implicit conversion
        GPUBlas() : handle() {}
        GPUBlas(GPUBlasHandle handle) : handle(handle) {}

        auto destroy() -> void;

        operator GPUBlasHandle() const { return handle; }
    };

    struct GPUBindGroup : public GPUObjectBase
    {
        GPUBindGroupHandle handle;

        // NOTE: no manual deletion of GPUBindGroup,
        // because these are automatically recycled by GC.

        // implicit conversion
        GPUBindGroup() : handle() {}
        GPUBindGroup(GPUBindGroupHandle handle) : handle(handle) {}

        operator GPUBindGroupHandle() const { return handle; }
    };

    struct GPUBindGroupLayout : public GPUObjectBase
    {
        GPUBindGroupLayoutHandle handle;

        // implicit conversion
        GPUBindGroupLayout() : handle() {}
        GPUBindGroupLayout(GPUBindGroupLayoutHandle handle) : handle(handle) {}

        void destroy();

        operator GPUBindGroupLayoutHandle() const { return handle; }
    };

    struct GPUPipelineLayout : public GPUObjectBase
    {
        GPUPipelineLayoutHandle handle;

        // implicit conversion
        GPUPipelineLayout() : handle() {}
        GPUPipelineLayout(GPUPipelineLayoutHandle handle) : handle(handle) {}

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
        GPUCommandEncoderHandle handle;

        void insert_debug_marker(CString marker_label);

        void push_debug_group(CString group_label);

        void pop_debug_group();

        void wait(const GPUFence& fence, GPUBarrierSyncFlags sync) const;

        void signal(const GPUFence& fence, GPUBarrierSyncFlags sync) const;

        void set_pipeline(const GPURenderPipeline& pipeline) const;

        void set_pipeline(const GPUComputePipeline& pipeline) const;

        void set_pipeline(const GPURayTracingPipeline& pipeline) const;

        void set_bind_group(GPUIndex32 index, const GPUBindGroup& bind_group, const Vector<GPUBufferDynamicOffset>& dynamic_offsets = {}) const;

        void dispatch_workgroups(GPUSize32 x, GPUSize32 y = 1, GPUSize32 z = 1) const;

        void dispatch_workgroups_indirect(const GPUBuffer& indirect_buffer, GPUSize64 indirect_offset) const;

        void set_index_buffer(const GPUBuffer& buffer, GPUIndexFormat index_format, GPUSize64 offset = 0, GPUSize64 size = 0) const;

        void set_vertex_buffer(GPUIndex32 slot, const GPUBuffer& buffer, GPUSize64 offset = 0, GPUSize64 size = 0) const;

        void draw(GPUSize32 vertex_count, GPUSize32 instance_count = 1, GPUSize32 first_vertex = 0, GPUSize32 first_instance = 0) const;

        void draw_indexed(GPUSize32 index_count, GPUSize32 instance_count = 1, GPUSize32 first_index = 0, GPUSignedOffset32 base_vertex = 0, GPUSize32 first_instance = 0) const;

        void draw_indirect(const GPUBuffer& indirect_buffer, GPUSize64 indirect_offset, GPUSize32 draw_count) const;

        void draw_indexed_indirect(const GPUBuffer& indirect_buffer, GPUSize64 indirect_offset, GPUSize32 draw_count) const;

        void begin_render_pass(const GPURenderPassDescriptor& descriptor) const;

        void end_render_pass() const;

        void copy_buffer_to_buffer(const GPUBuffer& source, const GPUBuffer& destination, GPUSize64 size) const;

        void copy_buffer_to_buffer(const GPUBuffer& source, GPUSize64 source_offset, const GPUBuffer& destination, GPUSize64 destination_offset, GPUSize64 size) const;

        void copy_buffer_to_texture(const GPUTexelCopyBufferInfo& source, const GPUTexelCopyTextureInfo& destination, const GPUExtent3D& copy_size) const;

        void copy_texture_to_buffer(const GPUTexelCopyTextureInfo& source, const GPUTexelCopyBufferInfo& destination, const GPUExtent3D& copy_size) const;

        void copy_texture_to_texture(const GPUTexelCopyTextureInfo& source, const GPUTexelCopyTextureInfo& destination, const GPUExtent3D& copy_size) const;

        void clear_buffer(const GPUBuffer& buffer, GPUSize64 offset = 0, GPUSize64 size = 0) const;

        void set_viewport(float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f) const;

        void set_scissor_rect(GPUIntegerCoordinate x, GPUIntegerCoordinate y, GPUIntegerCoordinate w, GPUIntegerCoordinate h) const;

        void set_blend_constant(GPUColor color) const;

        void set_stencil_reference(GPUStencilValue reference) const;

        void begin_occlusion_query(GPUSize32 query_index) const;

        void end_occlusion_query() const;

        void write_timestamp(const GPUQuerySet& query_set, GPUSize32 query_index) const;

        void write_blas_properties(const GPUQuerySet& query_set, GPUSize32 query_index, const GPUBlas& blas) const;

        void resolve_query_set(GPUQuerySet query_set, GPUSize32 first_query, GPUSize32 query_count, const GPUBuffer& destination, GPUSize64 destination_offset) const;

        void resource_barrier(const GPUBufferBarrier& barrier) const;

        void resource_barrier(const Vector<GPUBufferBarrier>& barriers) const;

        void resource_barrier(const GPUTextureBarrier& barrier) const;

        void resource_barrier(const Vector<GPUTextureBarrier>& barriers) const;
    };

    struct GPUCommandBundle : public GPUCommandEncoder
    {
        operator GPUCommandEncoderHandle() const { return handle; }
    };

    struct GPUCommandBuffer : public GPUCommandEncoder
    {
        operator GPUCommandEncoderHandle() const { return handle; }

        void submit() const;

        void execute_bundles(const Vector<GPUCommandBundle>& bundles) const;
    };

    struct GPUSurfaceTexture : public GPUObjectBase
    {
        GPUTextureHandle     texture;
        GPUTextureViewHandle view;
        GPUFenceHandle       complete;
        GPUFenceHandle       available;
        bool                 suboptimal;

        void present() const;

        operator GPUTextureViewHandle() const { return view; }
    };

    struct GPUDevice : public GPUObjectBase
    {
        GPUAdapterInfo       adapter_info = {};
        GPUSupportedFeatures features     = {};

        auto create_fence() const -> GPUFence;

        auto create_buffer(const GPUBufferDescriptor& descriptor) const -> GPUBuffer;

        auto create_texture(const GPUTextureDescriptor& descriptor) const -> GPUTexture;

        auto create_sampler(const GPUSamplerDescriptor& descriptor) const -> GPUSampler;

        auto create_shader_module(const GPUShaderModuleDescriptor& descriptor) const -> GPUShaderModule;

        auto create_query_set(const GPUQuerySetDescriptor& descriptor) const -> GPUQuerySet;

        auto create_blas(const GPUBlasDescriptor& descriptor, const Vector<GPUBlasGeometrySizeDescriptor>& sizes) const -> GPUBlas;

        auto create_tlas(const GPUTlasDescriptor& descriptor) const -> GPUTlas;

        auto create_bind_group(const GPUBindGroupDescriptor& descriptor) const -> GPUBindGroup;

        auto create_bind_group(const GPUBindlessDescriptor& descriptor) const -> GPUBindGroup;

        auto create_bind_group_layout(const GPUBindGroupLayoutDescriptor& descriptor) const -> GPUBindGroupLayout;

        auto create_pipeline_layout(const GPUPipelineLayoutDescriptor& descriptor) const -> GPUPipelineLayout;

        auto create_render_pipeline(const GPURenderPipelineDescriptor& descriptor) const -> GPURenderPipeline;

        auto create_compute_pipeline(const GPUComputePipelineDescriptor& descriptor) const -> GPUComputePipeline;

        auto create_raytracing_pipeline(const GPURayTracingPipelineDescriptor& descriptor) const -> GPURayTracingPipeline;

        auto create_command_buffer(const GPUCommandBufferDescriptor& descriptor) const -> GPUCommandBuffer;

        auto create_command_bundle(const GPUCommandBundleDescriptor& descriptor) const -> GPUCommandBundle;

        auto wait() const -> void;

        auto wait(GPUFence fence) const -> void;

        auto destroy() const -> void;
    };

    struct GPUSurface : public GPUObjectBase
    {
        auto get_current_texture() const -> GPUSurfaceTexture;

        auto get_current_format() const -> GPUTextureFormat;

        auto get_current_extent() const -> GPUExtent2D;

        auto destroy() const -> void;
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
        RHIFlags     flags = 0;
        RHIBackend   backend;
        WindowHandle window = {};

        static GPUDevice& get_current_device()
        {
            static GPUDevice DEVICE = {};
            return DEVICE;
        }

        static GPUSurface& get_current_surface()
        {
            static GPUSurface SURFACE = {};
            return SURFACE;
        }

        static auto init(const RHIDescriptor& descriptor) -> OwnedResource<RHI>;

        static auto api() -> RenderAPI*;

        auto destroy() const -> void;

        auto request_adapter(const GPUAdapterDescriptor& descriptor = {}) const -> GPUAdapter;

        auto request_surface(const GPUSurfaceDescriptor& descriptor = {}) const -> GPUSurface;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_TYPES_H
