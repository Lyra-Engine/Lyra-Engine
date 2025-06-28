#include "D3D12Utils.h"

void D3D12Heap::Heap::init(uint capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
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

D3D12CPUDescriptor D3D12Heap::Heap::allocate_cpu()
{
    uint index = count;
    if (index++ >= capacity) {
        assert(!freed.empty());
        index = freed.back();
        freed.pop_back();
    }

    D3D12CPUDescriptor descriptor{};
    descriptor.index  = index; // record the descriptor index
    descriptor.handle = heap->GetCPUDescriptorHandleForHeapStart();
    descriptor.handle.ptr += increment * index;
    return descriptor;
}

D3D12GPUDescriptor D3D12Heap::Heap::allocate_gpu()
{
    uint index = count;
    if (index++ >= capacity) {
        assert(!freed.empty());
        index = freed.back();
        freed.pop_back();
    }

    D3D12GPUDescriptor descriptor{};
    descriptor.handle = heap->GetGPUDescriptorHandleForHeapStart();
    descriptor.handle.ptr += increment * index;
    return descriptor;
}

void D3D12Heap::Heap::recycle(uint index)
{
    freed.push_back(index);
}

void D3D12Heap::Heap::reset()
{
    // reset count and free'd
    count = 0;
    freed.clear();
}

void D3D12Heap::Heap::destroy()
{
    reset();

    heap->Release();
    heap = nullptr;
}

void D3D12Heap::init(uint capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    this->capacity = capacity;
    heap_type      = type;
    heap_flags     = flags;

    // reset current heap index
    heap_index = 0;
}

void D3D12Heap::reset()
{
    for (auto& heap : heaps)
        heap.reset();

    // reset current heap index
    heap_index = 0;
}

void D3D12Heap::destroy()
{
    for (auto& heap : heaps)
        heap.destroy();

    heaps.clear();

    // reset current heap index
    heap_index = 0;
}

D3D12CPUDescriptor D3D12Heap::allocate_cpu()
{
    heap_index = find_pool_index();

    // record the pool/heap index
    auto descriptor = heaps.at(heap_index).allocate_cpu();
    descriptor.pool = heap_index;
    return descriptor;
}

D3D12GPUDescriptor D3D12Heap::allocate_gpu()
{
    heap_index = find_pool_index();
    return heaps.at(heap_index).allocate_gpu();
}

void D3D12Heap::recycle(const D3D12CPUDescriptor& descriptor)
{
    auto& heap = heaps.at(descriptor.pool);
    heap.recycle(descriptor.index);
}

uint D3D12Heap::find_pool_index()
{
    for (size_t i = heap_index; i < heaps.size(); i++) {
        auto& heap = heaps.at(i);
        if (heap.count < capacity || !heap.freed.empty())
            return i;
    }

    heaps.push_back(Heap());
    heaps.back().init(capacity, heap_type, heap_flags);
    return static_cast<uint>(heaps.size()) - 1;
}
