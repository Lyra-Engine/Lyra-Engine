// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

D3D12Buffer::D3D12Buffer()
{
    // do nothing
}

D3D12Buffer::D3D12Buffer(const GPUBufferDescriptor& desc)
{
    // TODO: to examine if buffer usages are set correctly.

    this->size = desc.size;

    D3D12_HEAP_PROPERTIES heap_props = {};
    heap_props.Type                  = infer_heap_type(desc.usage);
    heap_props.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask      = 1;
    heap_props.VisibleNodeMask       = 1;

    D3D12_RESOURCE_DESC resource_desc = {};
    resource_desc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width               = desc.size;
    resource_desc.Height              = 1;
    resource_desc.DepthOrArraySize    = 1;
    resource_desc.MipLevels           = 1;
    resource_desc.SampleDesc.Count    = 1;
    resource_desc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Alignment           = 0;
    resource_desc.Flags               = infer_buffer_flags(desc.usage);

    auto rhi = get_rhi();
    ThrowIfFailed(rhi->device->CreateCommittedResource(
        &heap_props,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, // or COMMON/STATE_COPY_DEST depending on use
        nullptr,
        IID_PPV_ARGS(&buffer)));

    if (desc.mapped_at_creation) map();
}

void D3D12Buffer::destroy()
{
    if (buffer) {
        buffer->Release();
        buffer = nullptr;
    }
}

void D3D12Buffer::map(GPUSize64 offset, GPUSize64 size)
{
    D3D12_RANGE range = {};
    range.Begin       = offset;
    range.End         = offset + size;

    void* mapped = reinterpret_cast<void*>(mapped_data);
    buffer->Map(0, &range, &mapped);
    mapped_size = size;
}

void D3D12Buffer::unmap()
{
    if (mapped_data)
        buffer->Unmap(0, nullptr);
}
