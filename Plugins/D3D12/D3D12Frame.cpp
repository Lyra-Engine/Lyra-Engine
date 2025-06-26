// reference: https://alain.xyz/blog/raw-directx12

#include "D3D12Utils.h"

void D3D12Frame::init()
{
    auto rhi = get_rhi();

    inflight_fence = GPUFenceHandle(rhi->fences.add(D3D12Fence(true)));
    image_available_semaphore = GPUFenceHandle(rhi->fences.add(D3D12Fence(true)));
}

void D3D12Frame::destroy()
{
    auto rhi = get_rhi();

    fetch_resource(rhi->fences, inflight_fence).destroy();
    fetch_resource(rhi->fences, image_available_semaphore).destroy();
}
