// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

void D3D12CommandBuffer::wait(const D3D12Fence& fence, GPUBarrierSyncFlags)
{
    // NOTE: D3D12 does not support fine-grain control at pipeline stage level,
    // therefore sync flags is not used.

    wait_fences.push_back(FenceOps{
        fence.fence,
        fence.target,
    });
}

void D3D12CommandBuffer::signal(const D3D12Fence& fence, GPUBarrierSyncFlags)
{
    // NOTE: D3D12 does not support fine-grain control at pipeline stage level,
    // therefore sync flags is not used.

    signal_fences.push_back(FenceOps{
        fence.fence,
        fence.target++,
    });
}

void D3D12CommandBuffer::submit()
{
    // GPU wait - make the command queue wait for the fence (current value)
    for (auto& fence : wait_fences)
        command_queue->Wait(fence.fence, fence.value);

    // submit the command list to the queue
    ID3D12CommandList* command_lists[] = {command_buffer};
    command_queue->ExecuteCommandLists(1, command_lists);

    // signal from GPU - command queue will signal when it reaches this point
    for (auto& fence : signal_fences)
        command_queue->Signal(fence.fence, fence.value);
}

void D3D12CommandBuffer::begin()
{
    command_buffer->Reset(command_allocator, nullptr);
}

void D3D12CommandBuffer::end()
{
    ThrowIfFailed(command_buffer->Close());
}
