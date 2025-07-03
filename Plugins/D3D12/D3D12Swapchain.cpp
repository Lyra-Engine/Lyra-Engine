// reference: https://alain.xyz/blog/raw-directx12
#include <Lyra/Window/API.h>
#include <Lyra/Window/Types.h>

#include "D3D12Utils.h"

void D3D12SwapFrame::init(uint backbuffer_index, uint width, uint height)
{
    auto rhi = get_rhi();

    // create a name for this swapchain texture
    auto name = std::wstring(L"Swapchain Texture ") + std::to_wstring(backbuffer_index);

    // destroy existing handles
    destroy();

    // create fence handle
    D3D12Fence fence(true);

    // create texture
    D3D12Texture texture;
    texture.samples     = 1;
    texture.format      = infer_texture_format(rhi->surface_format);
    texture.area.width  = width;
    texture.area.height = height;
    texture.usages      = GPUTextureUsage::RENDER_ATTACHMENT | GPUTextureUsage::COPY_DST | GPUTextureUsage::COPY_DST;
    ThrowIfFailed(rhi->swapchain->GetBuffer(backbuffer_index, IID_PPV_ARGS(&texture.texture)));
    texture.texture->SetName(name.c_str());

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
    this->image_available_fence = GPUFenceHandle(rhi->fences.add(fence));
    this->texture               = GPUTextureHandle(rhi->textures.add(texture));
    this->view                  = GPUTextureViewHandle(rhi->views.add(view));
}

void D3D12SwapFrame::destroy()
{
    auto rhi = get_rhi();

    // clean up existing fence
    if (image_available_fence.valid()) {
        fetch_resource(rhi->fences, image_available_fence).destroy();
        rhi->fences.remove(image_available_fence.value);
    }

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
    rhi->surface_format = GPUTextureFormat::RGBA8UNORM;
    auto format         = infer_texture_format(rhi->surface_format);

    // present mode
    rhi->present_mode = infer_present_mode(desc.present_mode);

    // check if swapchain creation if necessary
    if (rhi->swapchain != nullptr) {
        // re-create render target attachments from swapchain
        rhi->swapchain->ResizeBuffers(num_backbuffers, width, height, format, 0);
    } else {
        // create swapchain
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.Width                 = width;
        swapchain_desc.Height                = height;
        swapchain_desc.Format                = format;
        swapchain_desc.Stereo                = false;
        swapchain_desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT; // D3D12 disallows UAV access on back buffer textures (no direct compute shader write)
        swapchain_desc.BufferCount           = num_backbuffers;
        swapchain_desc.AlphaMode             = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapchain_desc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.SampleDesc.Count      = 1;
        swapchain_desc.SampleDesc.Quality    = 0;

        IDXGISwapChain1* swapchain;
        ThrowIfFailed(rhi->factory->CreateSwapChainForHwnd(rhi->graphics_queue, (HWND)desc.window.native, &swapchain_desc, nullptr, nullptr, &swapchain));
        ThrowIfFailed(swapchain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&swapchain));
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

    // image available from swapchain frame
    image_available_fence = rhi->swapchain_frames.at(rhi->current_image_index).image_available_fence;

    // render complete from current frame
    auto& current_frame                 = rhi->current_frame();
    current_frame.frame_id              = rhi->current_frame_index;
    current_frame.image_available_fence = image_available_fence;
    render_complete_fence               = current_frame.render_complete_fence;

    // backbuffer
    uint  backbuffer_index   = rhi->swapchain->GetCurrentBackBufferIndex();
    auto& backbuffer         = rhi->swapchain_frames.at(backbuffer_index);
    rhi->current_image_index = backbuffer_index;

    // wait before current frame is usable again
    current_frame.wait();
    current_frame.reset();

    // update swapchain view (this must be done after current_image_index is updated)
    texture = backbuffer.texture;
    view    = backbuffer.view;

    return true;
}

bool api::present_curr_frame()
{
    auto rhi = get_rhi();

    // query the current frame (also update the frame index)
    auto& frame = rhi->current_frame();

    // query the fences
    auto& image_available_fence = fetch_resource(rhi->fences, frame.image_available_fence);
    auto& render_complete_fence = fetch_resource(rhi->fences, frame.render_complete_fence);

    // check if nothing has been down, insert dummy workloads if needed
    if (frame.allocated_command_buffers.empty()) {
        auto  command_buffer_handle = frame.allocate(GPUQueueType::COMPUTE, true);
        auto& command_buffer        = frame.command(command_buffer_handle.value);
        command_buffer.begin();
        default_swapchain_image_barrier(command_buffer.command_buffer);
        command_buffer.end();
        command_buffer.wait(image_available_fence, GPUBarrierSync::NONE);
        command_buffer.signal(render_complete_fence, GPUBarrierSync::ALL);
        command_buffer.submit();
    }

    // signal the image available for current frame
    auto& backbuffer = rhi->swapchain_frames.at(rhi->current_image_index);
    auto& fence      = fetch_resource(rhi->fences, backbuffer.image_available_fence);
    rhi->graphics_queue->Signal(fence.fence, fence.target);

    // present to swapchain
    rhi->swapchain->Present(rhi->present_mode.sync_interval, rhi->present_mode.sync_flags);
    rhi->current_frame_index++;
    return true;
}
