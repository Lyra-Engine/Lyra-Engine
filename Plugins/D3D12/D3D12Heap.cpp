#include "D3D12Utils.h"

#pragma region D3D12HeapCPUUtils
void D3D12HeapCPUUtils::init(uint capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    // describe and create a render target view (RTV) descriptor heap.
    auto rhi = get_rhi();

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors             = capacity;
    heap_desc.Type                       = type;
    heap_desc.Flags                      = flags;
    ThrowIfFailed(rhi->device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap)));

    // capacity
    this->capacity = capacity;

    // descriptor size
    increment = rhi->device->GetDescriptorHandleIncrementSize(type);

    reset();
}

void D3D12HeapCPUUtils::reset()
{
    // reset count and free'd
    count = 0;
    freed.clear();
}

void D3D12HeapCPUUtils::destroy()
{
    reset();

    heap->Release();
    heap = nullptr;
}

void D3D12HeapCPUUtils::recycle(uint index)
{
    freed.push_back(index);
}

D3D12CPUDescriptor D3D12HeapCPUUtils::allocate()
{
    uint index;
    if (freed.empty()) {
        index = count++; // update count
    } else {
        index = freed.back();
        freed.pop_back();
    }

    D3D12CPUDescriptor descriptor{};
    descriptor.index  = index; // record the descriptor index
    descriptor.handle = heap->GetCPUDescriptorHandleForHeapStart();
    descriptor.handle.ptr += increment * index;
    return descriptor;
}
#pragma endregion D3D12HeapCPUUtils

#pragma region D3D12HeapCPU
void D3D12HeapCPU::init(uint capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    this->capacity = capacity;
    heap_type      = type;
    heap_flags     = flags;

    // reset current heap index
    heap_index = 0;
}

void D3D12HeapCPU::reset()
{
    for (auto& heap : heaps)
        heap.reset();

    // reset current heap index
    heap_index = 0;
}

void D3D12HeapCPU::destroy()
{
    for (auto& heap : heaps)
        heap.destroy();

    heaps.clear();

    // reset current heap index
    heap_index = 0;
}

D3D12CPUDescriptor D3D12HeapCPU::allocate()
{
    heap_index = find_pool_index();

    // record the pool/heap index
    auto descriptor = heaps.at(heap_index).allocate();
    descriptor.pool = heap_index;
    return descriptor;
}

void D3D12HeapCPU::recycle(const D3D12CPUDescriptor& descriptor)
{
    auto& heap = heaps.at(descriptor.pool);
    heap.recycle(descriptor.index);
}

uint D3D12HeapCPU::find_pool_index()
{
    for (size_t i = heap_index; i < heaps.size(); i++) {
        auto& heap = heaps.at(i);
        if (heap.count < capacity || !heap.freed.empty())
            return i;
    }

    heaps.emplace_back();
    heaps.back().init(capacity, heap_type, heap_flags);
    return static_cast<uint>(heaps.size()) - 1;
}
#pragma endregion D3D12HeapCPU

#pragma region D3D12HeapGPU
void D3D12HeapGPU::init(uint capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    // describe and create a render target view (RTV) descriptor heap.
    auto rhi = get_rhi();

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors             = capacity;
    heap_desc.Type                       = type;
    heap_desc.Flags                      = flags | D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(rhi->device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap)));

    // capacity
    this->capacity = capacity;

    // descriptor size
    increment = rhi->device->GetDescriptorHandleIncrementSize(type);

    reset();
}

void D3D12HeapGPU::reset()
{
    // reset count
    count = 0;
}

void D3D12HeapGPU::destroy()
{
    reset();

    heap->Release();
    heap = nullptr;
}

uint D3D12HeapGPU::allocate(uint allocate_count)
{
    // get the current base register
    uint index = count;

    // update count
    count += allocate_count;

    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12HeapGPU::cpu(uint index) const
{
    auto descriptor = heap->GetCPUDescriptorHandleForHeapStart();
    descriptor.ptr += index * increment;
    return descriptor;
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12HeapGPU::gpu(uint index) const
{
    auto descriptor = heap->GetGPUDescriptorHandleForHeapStart();
    descriptor.ptr += index * increment;
    return descriptor;
}

#pragma endregion D3D12HeapGPU
