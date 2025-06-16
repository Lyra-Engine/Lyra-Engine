#ifndef LYRA_LIBRARY_RENDER_RHI_INITS_H
#define LYRA_LIBRARY_RENDER_RHI_INITS_H

#include <Render/RHI/Enums.h>
#include <Render/RHI/Utils.h>
#include <Render/RHI/Descs.h>

namespace lyra::rhi
{
    inline GPUTextureBarrier transition_base_template(rhi::GPUTextureHandle texture, uint32_t base_array_layer = 0, uint32_t layer_count = 1, uint32_t base_mip_level = 0, uint32_t level_count = 1)
    {
        auto barrier                          = GPUTextureBarrier{};
        barrier.texture                       = texture;
        barrier.subresources.base_array_layer = base_array_layer;
        barrier.subresources.base_mip_level   = base_mip_level;
        barrier.subresources.layer_count      = layer_count;
        barrier.subresources.level_count      = level_count;
        return barrier;
    }

    inline GPUTextureBarrier transition_undefined_to_present(rhi::GPUTextureHandle texture, uint32_t base_array_layer = 0, uint32_t layer_count = 1, uint32_t base_mip_level = 0, uint32_t level_count = 1)
    {
        auto barrier       = transition_base_template(texture, base_array_layer, layer_count, base_mip_level, level_count);
        barrier.src_layout = GPUBarrierLayout::UNDEFINED;
        barrier.dst_layout = GPUBarrierLayout::PRESENT;
        barrier.src_sync   = GPUBarrierSync::NONE;
        barrier.dst_sync   = GPUBarrierSync::NONE;
        barrier.src_access = GPUBarrierAccess::NO_ACCESS;
        barrier.dst_access = GPUBarrierAccess::NO_ACCESS;
        return barrier;
    }

    inline GPUTextureBarrier transition_undefined_to_color_attachment(rhi::GPUTextureHandle texture, uint32_t base_array_layer = 0, uint32_t layer_count = 1, uint32_t base_mip_level = 0, uint32_t level_count = 1)
    {
        auto barrier       = transition_base_template(texture, base_array_layer, layer_count, base_mip_level, level_count);
        barrier.src_layout = GPUBarrierLayout::UNDEFINED;
        barrier.dst_layout = GPUBarrierLayout::RENDER_TARGET;
        barrier.src_sync   = GPUBarrierSync::NONE;
        barrier.dst_sync   = GPUBarrierSync::RENDER_TARGET;
        barrier.src_access = GPUBarrierAccess::NO_ACCESS;
        barrier.dst_access = GPUBarrierAccess::RENDER_TARGET;
        return barrier;
    }

    inline GPUTextureBarrier transition_color_attachment_to_present(rhi::GPUTextureHandle texture, uint32_t base_array_layer = 0, uint32_t layer_count = 1, uint32_t base_mip_level = 0, uint32_t level_count = 1)
    {
        auto barrier       = transition_base_template(texture, base_array_layer, layer_count, base_mip_level, level_count);
        barrier.src_layout = GPUBarrierLayout::RENDER_TARGET;
        barrier.dst_layout = GPUBarrierLayout::PRESENT;
        barrier.src_sync   = GPUBarrierSync::RENDER_TARGET;
        barrier.dst_sync   = GPUBarrierSync::ALL;
        barrier.src_access = GPUBarrierAccess::RENDER_TARGET;
        barrier.dst_access = GPUBarrierAccess::NO_ACCESS;
        return barrier;
    }

    inline GPUTextureBarrier transition_color_attachment_to_copy_src(rhi::GPUTextureHandle texture, uint32_t base_array_layer = 0, uint32_t layer_count = 1, uint32_t base_mip_level = 0, uint32_t level_count = 1)
    {
        auto barrier       = transition_base_template(texture, base_array_layer, layer_count, base_mip_level, level_count);
        barrier.src_layout = GPUBarrierLayout::RENDER_TARGET;
        barrier.dst_layout = GPUBarrierLayout::COPY_SOURCE;
        barrier.src_sync   = GPUBarrierSync::RENDER_TARGET;
        barrier.dst_sync   = GPUBarrierSync::COPY;
        barrier.src_access = GPUBarrierAccess::RENDER_TARGET;
        barrier.dst_access = GPUBarrierAccess::COPY_SOURCE;
        return barrier;
    }

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_INITS_H
