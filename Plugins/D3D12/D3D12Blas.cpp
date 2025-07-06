#include "D3D12Utils.h"

D3D12Blas::D3D12Blas()
{
    blas = nullptr;
}

D3D12Blas::D3D12Blas(const GPUBlasDescriptor& desc, const Vector<GPUBlasGeometrySizeDescriptor>& sizes)
{
    assert(!!!"Blas is not supported yet!");
}

void D3D12Blas::destroy()
{
    if (blas != nullptr) {
        blas->Release();
        blas = nullptr;
    }
}
