// reference: https://alain.xyz/blog/raw-directx12
#include <Lyra/Window/API.h>
#include <Lyra/Window/Types.h>

#include "D3D12Utils.h"

void D3D12SwapFrame::init(uint backbuffer_index, uint width, uint height)
{
    auto rhi = get_rhi();

    // destroy existing handles
    destroy();

    // create texture
    D3D12Texture texture;
    texture.samples     = 1;
    texture.format      = infer_texture_format(rhi->surface_format);
    texture.area.width  = width;
    texture.area.height = height;
    texture.usages      = GPUTextureUsage::RENDER_ATTACHMENT | GPUTextureUsage::COPY_DST | GPUTextureUsage::COPY_DST | GPUTextureUsage::STORAGE_BINDING;
    ThrowIfFailed(rhi->swapchain->GetBuffer(backbuffer_index, IID_PPV_ARGS(&texture.texture)));

    // create texture view
    GPUTextureViewDescriptor view_desc = {};
    view_desc.format                   = rhi->surface_format;
    view_desc.dimension                = GPUTextureViewDimension::x2D;
    view_desc.aspect                   = GPUTextureAspect::COLOR;
    view_desc.array_layer_count        = 1;
    view_desc.base_array_layer         = 0;
    view_desc.base_mip_level           = 0;
    view_desc.mip_level_count          = 1;
    view_desc.usage                    = 0;
    view_desc.label                    = "swapchain-view";
    D3D12TextureView view(texture, view_desc);

    // fill in swap frame
    this->texture = GPUTextureHandle(rhi->textures.add(texture));
    this->view    = GPUTextureViewHandle(rhi->views.add(view));
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
}

bool api::create_surface(GPUSurface& surface, const GPUSurfaceDescriptor& desc)
{
    auto rhi = get_rhi();

    // query window size
    uint width, height;
    Window::api()->get_window_size(desc.window, width, height);
    rhi->surface_extent.width  = width;
    rhi->surface_extent.height = height;

    // query number of backbuffers
    uint num_backbuffers = desc.frames_inflight;

    // query compatible backbuffer format
    rhi->surface_format = GPUTextureFormat::BGRA8UNORM_SRGB;
    auto format         = infer_texture_format(rhi->surface_format);

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
        rhi->swapchain_frames.at(i).init(i, width, height);

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

void default_swapchain_image_barrier(ID3D12GraphicsCommandList* command_buffer)
{
    auto rhi = get_rhi();

    auto& frame   = rhi->swapchain_frames.at(rhi->current_image_index);
    auto& texture = fetch_resource(rhi->textures, frame.texture);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.pResource   = texture.texture;

    command_buffer->ResourceBarrier(1, &barrier);
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

bool api::acquire_next_frame(GPUTextureHandle& texture, GPUTextureViewHandle& view, GPUFenceHandle& image_available_fence, GPUFenceHandle& render_complete_fence, bool& suboptimal)
{
    auto rhi = get_rhi();

    // initialize suboptimal
    suboptimal = false;

    // acquire next frame
    rhi->current_image_index = rhi->swapchain->GetCurrentBackBufferIndex();

    auto& backbuffer = rhi->swapchain_frames.at(rhi->current_image_index);

    // query the current frame
    auto& frame    = rhi->current_frame();
    frame.frame_id = rhi->current_frame_index;

    // wait for inflght frame to complete
    frame.wait();
    frame.reset();

    // update swapchain view (this must be done after current_image_index is updated)
    texture               = backbuffer.texture;
    view                  = backbuffer.view;
    render_complete_fence = frame.render_complete_fence;

    return true;
}

bool api::present_curr_frame()
{
    auto rhi = get_rhi();

    // query the current frame (also update the frame index)
    auto& frame = rhi->current_frame();

    // query the semaphores
    auto& render_complete_fence = fetch_resource(rhi->fences, frame.render_complete_fence);

    // check if nothing has been down, insert dummy workloads if needed
    if (frame.allocated_command_buffers.empty()) {
        auto  command_buffer_handle = frame.allocate(GPUQueueType::COMPUTE, true);
        auto& command_buffer        = frame.allocated_command_buffers.at(command_buffer_handle.value);
        command_buffer.begin();
        default_swapchain_image_barrier(command_buffer.command_buffer);
        command_buffer.end();
        command_buffer.signal(render_complete_fence, GPUBarrierSync::ALL);
        command_buffer.submit();
    }

    // present to swapchain
    rhi->swapchain->Present(rhi->present_mode.sync_interval, rhi->present_mode.sync_flags);
    rhi->current_frame_index++;
    return true;
}
