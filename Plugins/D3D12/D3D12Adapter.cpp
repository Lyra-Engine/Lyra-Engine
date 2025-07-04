// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

bool api::create_adapter(GPUAdapter& adapter, const GPUAdapterDescriptor& desc)
{
    auto rhi = get_rhi();

    // create adapter
    for (UINT adapter_index = 0;; ++adapter_index) {
        IDXGIAdapter1* current_adapter = nullptr;

        // try to get the next adapter
        if (DXGI_ERROR_NOT_FOUND == rhi->factory->EnumAdapters1(adapter_index, &current_adapter))
            break; // no more adapters

        DXGI_ADAPTER_DESC1 adapter_desc;
        current_adapter->GetDesc1(&adapter_desc);

        // ignore Basic Render Driver adapter
        if (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            current_adapter->Release();
            continue;
        }

        // check if the adapter supports Direct3D 12
        ID3D12Device* current_device = nullptr;
        if (SUCCEEDED(D3D12CreateDevice(current_adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&current_device)))) {
            // this adapter works - store it
            current_device->Release();
            rhi->adapter = current_adapter;
            break;
        }

        // This adapter doesn't work, release it
        current_adapter->Release();
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
