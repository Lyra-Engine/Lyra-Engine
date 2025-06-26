// reference: https://alain.xyz/blog/raw-directx12
#include "D3D12Utils.h"

D3D12Texture::D3D12Texture()
{
    // do nothing
}

D3D12Texture::D3D12Texture(ID3D12Resource* texture) : texture(texture)
{
    // do nothing else
}

D3D12Texture::D3D12Texture(const GPUTextureDescriptor& desc)
{
    D3D12_HEAP_PROPERTIES heap_props = {};
    heap_props.Type                  = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC tex_desc = {};
    tex_desc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    tex_desc.Width               = desc.size.width;
    tex_desc.Height              = desc.size.height;
    tex_desc.DepthOrArraySize    = 1;
    tex_desc.MipLevels           = 1;
    tex_desc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.SampleDesc.Count    = 1;
    tex_desc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    tex_desc.Flags               = infer_texture_flags(desc.usage, desc.format);

    auto rhi = get_rhi();
    ThrowIfFailed(rhi->device->CreateCommittedResource(
        &heap_props,
        D3D12_HEAP_FLAG_NONE,
        &tex_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&texture)));
}

void D3D12Texture::destroy()
{
    if (texture) {
        texture->Release();
        texture = nullptr;
    }
}
