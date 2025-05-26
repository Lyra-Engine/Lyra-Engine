#ifndef LYRA_LIBRARY_RENDER_RHI_MIXIN_H
#define LYRA_LIBRARY_RENDER_RHI_MIXIN_H

#include <Render/RHI/Utils.h>

namespace lyra::rhi
{
    struct GPUCommandsMixin
    {
    };

    struct GPUDebugCommandsMixin
    {
        virtual void push_debug_group(CString group_label) = 0;

        virtual void pop_debug_group() = 0;

        virtual void insert_debug_marker(CString marker_label) = 0;
    };

    struct GPUBindingCommandsMixin
    {
        virtual void set_bind_group(
            GPUIndex32                            index,
            GPUBindGroupHandle                    bind_group,
            const Vector<GPUBufferDynamicOffset>& dynamic_offsets = {}) = 0;

        virtual void set_bind_group(
            GPUIndex32         index,
            GPUBindGroupHandle bind_group,
            const uint32_t*    dynamic_offsets_data,
            GPUSize64          dynamic_offsets_data_start,
            GPUSize32          dynamic_offsets_data_length) = 0;
    };

    struct GPURenderCommandsMixin
    {
        virtual void set_pipeline(GPURenderPipelineHandle pipeline) = 0;

        virtual void set_index_buffer(
            GPUBufferHandle buffer,
            GPUIndexFormat  index_format,
            GPUSize64       offset = 0,
            GPUSize64       size   = 0) = 0;

        virtual void set_vertex_buffer(
            GPUIndex32      slot,
            GPUBufferHandle buffer,
            GPUSize64       offset = 0,
            GPUSize64       size   = 0) = 0;

        virtual void draw(
            GPUSize32 vertex_count,
            GPUSize32 instance_count = 1,
            GPUSize32 first_vertex   = 0,
            GPUSize32 first_instance = 0) = 0;

        virtual void drawIndexed(
            GPUSize32         index_count,
            GPUSize32         instance_count = 1,
            GPUSize32         first_index    = 0,
            GPUSignedOffset32 base_vertex    = 0,
            GPUSize32         first_instance = 0) = 0;

        virtual void draw_indirect(
            GPUBufferHandle indirect_buffer,
            GPUSize64       indirect_offset) = 0;

        virtual void draw_indexed_indirect(
            GPUBufferHandle indirect_buffer,
            GPUSize64       indirect_offset) = 0;
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_MIXIN_H
