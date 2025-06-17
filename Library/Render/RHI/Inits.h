#ifndef LYRA_LIBRARY_RENDER_RHI_INITS_H
#define LYRA_LIBRARY_RENDER_RHI_INITS_H

#include <Render/RHI/Enums.h>
#include <Render/RHI/Utils.h>
#include <Render/RHI/Descs.h>

namespace lyra::rhi
{
    struct TransitionState
    {
        GPUBarrierLayout layout;
        GPUBarrierSync   sync;
        GPUBarrierAccess access;
    };

    TransitionState undefined_state();
    TransitionState present_src_state();
    TransitionState color_attachment_state();
    TransitionState copy_src_state();
    TransitionState copy_dst_state();

    GPUTextureBarrier state_transition(
        GPUTextureHandle       texture,
        const TransitionState& src_state,
        const TransitionState& dst_state,
        uint32_t               base_array_layer = 0,
        uint32_t               layer_count      = 1,
        uint32_t               base_mip_level   = 0,
        uint32_t               level_count      = 1);

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_INITS_H
