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

D3D12Descriptor D3D12Heap::Heap::allocate()
{
    uint index = count;
    if (index++ >= capacity) {
        assert(!freed.empty());
        index = freed.back();
        freed.pop_back();
    }

    D3D12Descriptor descriptor = {};
    descriptor.cpu_handle      = heap->GetCPUDescriptorHandleForHeapStart();
    descriptor.gpu_handle      = heap->GetGPUDescriptorHandleForHeapStart();
    descriptor.cpu_handle.ptr += increment * index;
    descriptor.gpu_handle.ptr += increment * index;
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

D3D12Descriptor D3D12Heap::allocate()
{
    heap_index = find_pool_index();
    return heaps.at(heap_index).allocate();
}

void D3D12Heap::recycle(const D3D12Descriptor& descriptor)
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
