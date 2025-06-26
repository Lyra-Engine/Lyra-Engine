// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

bool api::create_device(const GPUDeviceDescriptor& desc)
{
    auto rhi = get_rhi();

    // create device
    ThrowIfFailed(D3D12CreateDevice(rhi->adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&rhi->device)));

    // fetch debug device
    if (rhi->rhiflags.contains(RHIFlag::DEBUG))
        ThrowIfFailed(rhi->device->QueryInterface(&rhi->debug_device));

    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.NodeMask                 = 0; // usually 0 for single GPU

    // create graphics queue
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(rhi->device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&rhi->graphics_queue)));

    // create compute queue
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    ThrowIfFailed(rhi->device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&rhi->compute_queue)));

    // create transfer queue
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    ThrowIfFailed(rhi->device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&rhi->transfer_queue)));

    // create descriptor heaps
    rhi->rtv_heap.init(512, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    rhi->dsv_heap.init(512, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    rhi->sampler_heap.init(512, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    rhi->cbv_srv_uav_heap.init(512, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    return true;
}

void api::delete_device()
{
    auto rhi = get_rhi();

    if (rhi->transfer_queue) {
        rhi->transfer_queue->Release();
        rhi->transfer_queue = nullptr;
    }

    if (rhi->graphics_queue) {
        rhi->graphics_queue->Release();
        rhi->graphics_queue = nullptr;
    }

    if (rhi->compute_queue) {
        rhi->compute_queue->Release();
        rhi->compute_queue = nullptr;
    }

    if (rhi->device) {
        rhi->device->Release();
        rhi->device = nullptr;
    }
}
