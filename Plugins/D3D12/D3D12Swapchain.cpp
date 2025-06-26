// reference: https://alain.xyz/blog/raw-directx12
#include <Lyra/Window/API.h>
#include <Lyra/Window/Types.h>

#include "D3D12Utils.h"

void D3D12SwapFrame::init(int backbuffer_index)
{
    auto rhi = get_rhi();

    // destroy existing handles
    destroy();

    // create texture
    D3D12Texture texture;
    ThrowIfFailed(rhi->swapchain->GetBuffer(backbuffer_index, IID_PPV_ARGS(&texture.texture)));

    // TODO: create texture view
    D3D12TextureView view;

    // create fence
    D3D12Fence fence(true);

    // fill in swap frame
    this->texture                   = GPUTextureHandle(rhi->textures.add(texture));
    this->view                      = GPUTextureViewHandle(rhi->views.add(view));
    this->render_complete_semaphore = GPUFenceHandle(rhi->fences.add(fence));
}

void D3D12SwapFrame::destroy()
{
    auto rhi = get_rhi();

    // clean up existing texture
    if (texture.valid()) {
        fetch_resource(rhi->textures, texture).destroy();
        rhi->textures.remove(texture.value);
    }

    // clean up existing texture view
    if (view.valid()) {
        fetch_resource(rhi->views, view).destroy();
        rhi->views.remove(view.value);
    }

    // clean up existing fence
    if (render_complete_semaphore.valid()) {
        fetch_resource(rhi->fences, render_complete_semaphore).destroy();
        rhi->views.remove(view.value);
    }
}

bool api::create_surface(GPUSurface& surface, const GPUSurfaceDescriptor& desc)
{
    auto rhi = get_rhi();

    // query window size
    uint width, height;
    Window::api()->get_window_size(desc.window, width, height);

    // query number of backbuffers
    uint num_backbuffers = desc.frames_inflight;

    // query compatible backbuffer format
    auto format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // check if swapchain creation if necessary
    if (rhi->swapchain != nullptr) {
        // re-create render target attachments from swapchain
        rhi->swapchain->ResizeBuffers(num_backbuffers, width, height, format, 0);
    } else {
        // create swapchain
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.BufferCount           = num_backbuffers;
        swapchain_desc.Width                 = width;
        swapchain_desc.Height                = height;
        swapchain_desc.Format                = format;
        swapchain_desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.SampleDesc.Count      = 1;

        IDXGISwapChain1* swapchain;
        ThrowIfFailed(rhi->factory->CreateSwapChainForHwnd(rhi->device, (HWND)desc.window.native, &swapchain_desc, nullptr, nullptr, &swapchain));
        ThrowIfFailed(rhi->swapchain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&swapchain));
        rhi->swapchain = (IDXGISwapChain3*)swapchain;
    }

    // destroy swap frames
    for (auto& frame : rhi->swapchain_frames)
        frame.destroy();

    // initialize swap frame
    rhi->swapchain_frames.clear();
    rhi->swapchain_frames.resize(num_backbuffers);
    for (uint i = 0; i < num_backbuffers; i++)
        rhi->swapchain_frames.at(i).init(i);

    // initialize frame
    uint num_frames = static_cast<uint>(rhi->frames.size());
    if (num_backbuffers < num_frames) {
        for (uint i = num_backbuffers; i < num_frames; i++)
            rhi->frames.at(i).destroy();
        rhi->frames.resize(num_backbuffers);
    } else {
        rhi->frames.resize(num_backbuffers);
        for (uint i = num_frames; i < num_backbuffers; i++)
            rhi->frames.at(i).init();
    }

    // reset frame / image indices
    rhi->current_frame_index = 0;
    rhi->current_image_index = 0;
    return true;
}

void api::delete_surface()
{
    auto rhi = get_rhi();

    for (auto& swap_frame : rhi->swapchain_frames)
        swap_frame.destroy();
    rhi->swapchain_frames.clear();

    if (rhi->swapchain) {
        rhi->swapchain->Release();
        rhi->swapchain = nullptr;
    }
}

// bool api::get_surface_extent(GPUExtent2D& extent)
// {
// }
//
// bool api::get_surface_format(GPUTextureFormat& format)
// {
// }
