// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

bool api::create_adapter(GPUAdapter& adapter, const GPUAdapterDescriptor& desc)
{
    auto rhi = get_rhi();

    // Create Adapter
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != rhi->factory->EnumAdapters1(adapterIndex, &rhi->adapter); ++adapterIndex) {
        DXGI_ADAPTER_DESC1 adapter_desc;
        rhi->adapter->GetDesc1(&adapter_desc);

        // ignore Basic Render Driver adapter
        if (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        // check if the adapter supports Direct3D 12, and use that for the rest of the application
        if (SUCCEEDED(D3D12CreateDevice(rhi->adapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            break;

        // Else we won't use this iteration's adapter, so release it
        rhi->adapter->Release();
    }

    return rhi->adapter != nullptr;
}

void api::delete_adapter()
{
    auto rhi = get_rhi();

    if (rhi->adapter) {
        rhi->adapter->Release();
        rhi->adapter = nullptr;
    }
}
