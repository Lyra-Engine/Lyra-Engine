#include <Render/RHI/Inits.h>

using namespace lyra::rhi;

TransitionState lyra::rhi::undefined_state()
{
    TransitionState state{};
    state.layout = GPUBarrierLayout::UNDEFINED;
    state.sync   = GPUBarrierSync::NONE;
    state.access = GPUBarrierAccess::NO_ACCESS;
    return state;
}

TransitionState lyra::rhi::present_src_state()
{
    TransitionState state{};
    state.layout = GPUBarrierLayout::PRESENT;
    state.sync   = GPUBarrierSync::NONE;
    state.access = GPUBarrierAccess::NO_ACCESS;
    return state;
}

TransitionState lyra::rhi::color_attachment_state()
{
    TransitionState state{};
    state.layout = GPUBarrierLayout::RENDER_TARGET;
    state.sync   = GPUBarrierSync::RENDER_TARGET;
    state.access = GPUBarrierAccess::RENDER_TARGET;
    return state;
}

TransitionState lyra::rhi::copy_src_state()
{
    TransitionState state{};
    state.layout = GPUBarrierLayout::COPY_SOURCE;
    state.sync   = GPUBarrierSync::COPY;
    state.access = GPUBarrierAccess::COPY_SOURCE;
    return state;
}

TransitionState lyra::rhi::copy_dst_state()
{
    TransitionState state{};
    state.layout = GPUBarrierLayout::COPY_DEST;
    state.sync   = GPUBarrierSync::COPY;
    state.access = GPUBarrierAccess::COPY_DEST;
    return state;
}

GPUTextureBarrier lyra::rhi::state_transition(
    GPUTextureHandle       texture,
    const TransitionState& src_state,
    const TransitionState& dst_state,
    uint32_t               base_array_layer,
    uint32_t               layer_count,
    uint32_t               base_mip_level,
    uint32_t               level_count)
{
    auto barrier                          = GPUTextureBarrier{};
    barrier.texture                       = texture;
    barrier.subresources.base_array_layer = base_array_layer;
    barrier.subresources.base_mip_level   = base_mip_level;
    barrier.subresources.layer_count      = layer_count;
    barrier.subresources.level_count      = level_count;
    barrier.src_layout                    = src_state.layout;
    barrier.dst_layout                    = dst_state.layout;
    barrier.src_access                    = src_state.access;
    barrier.dst_access                    = dst_state.access;
    barrier.src_sync                      = src_state.sync;
    barrier.dst_sync                      = dst_state.sync;
    return barrier;
}
