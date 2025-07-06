#include "D3D12Utils.h"

D3D12QuerySet::D3D12QuerySet()
{
    pool = nullptr;
}

D3D12QuerySet::D3D12QuerySet(const GPUQuerySetDescriptor& desc)
{
    assert(!!!"QuerySet is not supported yet!");
}

void D3D12QuerySet::destroy()
{
    if (pool != nullptr) {
        pool->Release();
        pool = nullptr;
    }
}
