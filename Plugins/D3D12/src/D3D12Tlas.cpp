#include "D3D12Utils.h"

D3D12Tlas::D3D12Tlas()
{
    tlas = nullptr;
}

D3D12Tlas::D3D12Tlas(const GPUTlasDescriptor& desc)
{
    assert(!!!"Tlas is not supported yet!");
}

void D3D12Tlas::destroy()
{
    if (tlas != nullptr) {
        tlas->Release();
        tlas = nullptr;
    }
}
