#include <algorithm>
#include "VkUtils.h"

void VulkanRHI::create_swapchain()
{
    auto rhi = get_rhi();

    SwapchainSupportDetails swapchain_support = query_swapchain_support(rhi->adapter, rhi->surface);
    VkSurfaceFormatKHR      surface_format    = choose_swap_surface_format(swapchain_support.formats);
    VkPresentModeKHR        present_mode      = choose_swap_present_mode(swapchain_support.present_modes);
    VkExtent2D              extent            = choose_swap_extent(surface_desc, swapchain_support.capabilities);

    uint32_t image_count = std::clamp(
        surface_desc.frames_inflight,
        swapchain_support.capabilities.minImageCount,
        swapchain_support.capabilities.maxImageCount);

    auto create_info             = VkSwapchainCreateInfoKHR{};
    create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface          = rhi->surface;
    create_info.minImageCount    = image_count;
    create_info.imageFormat      = surface_format.format;
    create_info.imageColorSpace  = surface_format.colorSpace;
    create_info.imageExtent      = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    create_info.clipped          = VK_TRUE;
    create_info.presentMode      = present_mode;
    create_info.preTransform     = swapchain_support.capabilities.currentTransform;
    create_info.compositeAlpha   = vkenum(surface_desc.alpha_mode);

    auto indices              = find_queue_family_indices(rhi->adapter, rhi->surface);
    uint queueFamilyIndices[] = {indices.graphics.value(), indices.present.value()};
    if (indices.graphics != indices.present) {
        create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queueFamilyIndices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VkSwapchainKHR old_swapchain = rhi->swapchain;
    if (rhi->swapchain != VK_NULL_HANDLE)
        create_info.oldSwapchain = old_swapchain;

    rhi->swapchain_colorspace = surface_format.colorSpace;
    rhi->swapchain_format     = surface_format.format;
    rhi->swapchain_extent     = extent;
    vk_check(rhi->vtable.vkCreateSwapchainKHR(rhi->device, &create_info, nullptr, &rhi->swapchain));

    // delete the old swapchain
    if (old_swapchain != VK_NULL_HANDLE)
        rhi->vtable.vkDestroySwapchainKHR(rhi->device, old_swapchain, nullptr);

    // get swapchain images
    uint count;
    vk_check(rhi->vtable.vkGetSwapchainImagesKHR(rhi->device, rhi->swapchain, &count, nullptr));

    Vector<VkImage> swapchain_images(count);
    vk_check(rhi->vtable.vkGetSwapchainImagesKHR(rhi->device, rhi->swapchain, &count, swapchain_images.data()));

    // clean up swapchain data if mismatching size
    if (rhi->swapchain_frames.size() != count) {
        for (auto& swap_frame : rhi->swapchain_frames)
            swap_frame.destroy();
        rhi->swapchain_frames.clear();
        rhi->swapchain_frames.resize(count);
    }

    // create swapchain data
    for (uint i = 0; i < count; i++)
        rhi->swapchain_frames.at(i).init(swapchain_images.at(i), surface_format.format, extent);

    // create logical frames in flight
    if (rhi->frames.empty()) {
        rhi->frames.resize(surface_desc.frames_inflight);
        for (auto& frame : rhi->frames)
            frame.init();
    }

    // reset frame / image indices
    rhi->current_frame_index = 0;
    rhi->current_image_index = 0;
}

void VulkanSwapFrame::init(VkImage image, VkFormat format, VkExtent2D extent)
{
    auto rhi = get_rhi();

    // clean up texture if already created
    if (this->texture.valid())
        fetch_resource(rhi->textures, texture).destroy();

    // clean up texture view if already created
    if (this->view.valid())
        fetch_resource(rhi->views, view).destroy();

    // reuse render complete semaphore if possible
    if (!render_complete_semaphore.valid())
        api::create_fence(render_complete_semaphore, VK_SEMAPHORE_TYPE_BINARY);

    // re-create texture
    auto texture    = VulkanTexture{};
    texture.image   = image;
    texture.aspects = VK_IMAGE_ASPECT_COLOR_BIT;
    this->texture   = rhi->textures.add(texture);

    // re-create new texture view
    auto view        = VulkanTextureView();
    auto create_info = VkImageViewCreateInfo{};
    {
        create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        create_info.image                           = image;
        create_info.format                          = format;
        create_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel   = 0;
        create_info.subresourceRange.levelCount     = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount     = 1;
    }
    vk_check(rhi->vtable.vkCreateImageView(rhi->device, &create_info, nullptr, &view.view));
    this->view  = rhi->views.add(view);
}

void VulkanSwapFrame::destroy()
{
    auto rhi = get_rhi();

    // texture does not need to be deleted (but we can do it to free up texture handles)
    fetch_resource(rhi->textures, texture).destroy();
    fetch_resource(rhi->views, view).destroy();
    fetch_resource(rhi->fences, render_complete_semaphore).destroy();
}

SwapchainSupportDetails query_swapchain_support(VkPhysicalDevice adapter, VkSurfaceKHR surface)
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter, surface, &details.capabilities);

    uint format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &format_count, nullptr);

    if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(adapter, surface, &format_count, details.formats.data());
    }

    uint present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, surface, &present_mode_count, nullptr);

    if (present_mode_count != 0) {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(adapter, surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

VkSurfaceFormatKHR choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats.at(0);
}

VkPresentModeKHR choose_swap_present_mode(const Vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(const GPUSurfaceDescriptor& desc, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    // query window size
    uint width, height;
    Window::api()->get_window_size(desc.window, width, height);

    VkExtent2D actual_extent;
    actual_extent.width  = width;
    actual_extent.height = height;
    actual_extent.width  = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return actual_extent;
}

void default_swapchain_image_barrier(VkCommandBuffer command_buffer)
{
    auto rhi = get_rhi();

    auto swap_frame = rhi->swapchain_frames.at(rhi->current_image_index);
    auto texture    = fetch_resource(rhi->textures, swap_frame.texture);

    auto barrier                            = VkImageMemoryBarrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext                           = nullptr;
    barrier.srcAccessMask                   = VK_ACCESS_NONE;
    barrier.dstAccessMask                   = VK_ACCESS_NONE;
    barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout                       = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_EXTERNAL;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_EXTERNAL;
    barrier.image                           = texture.image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    rhi->vtable.vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_NONE,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

// vulkan swapchain utils
bool api::acquire_next_frame(GPUTextureHandle& texture, GPUTextureViewHandle& view, GPUFenceHandle& image_available_fence, GPUFenceHandle& render_complete_fence, bool& suboptimal)
{
    auto rhi = get_rhi();

    // query the current frame
    auto& frame    = rhi->current_frame();
    frame.frame_id = rhi->current_frame_index;

    // wait for inflght frame to complete
    frame.wait();
    frame.reset();

    // initialize suboptimal
    suboptimal = false;

    // acquire next frame
    auto semaphore = fetch_resource(rhi->fences, frame.image_available_semaphore);
    auto result    = rhi->vtable.vkAcquireNextImageKHR(rhi->device, rhi->swapchain, UINT64_MAX, semaphore.semaphore, VK_NULL_HANDLE, &rhi->current_image_index);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
        // recreate the swapchain if window resizes or moved to other displays
        api::wait_idle();
        rhi->create_swapchain();
        suboptimal = true;
        return true;
    }
    vk_check(result);

    // update swapchain view (this must be done after current_image_index is updated)
    auto& swap_frame = rhi->swapchain_frames.at(rhi->current_image_index);
    texture          = swap_frame.texture;
    view             = swap_frame.view;

    // update fences (this must be done after current_image_index is updated)
    image_available_fence = rhi->current_frame().image_available_semaphore;
    render_complete_fence = rhi->current_image().render_complete_semaphore;

    // also tracks the render complete semaphore in frame (for consistent synchronization)
    frame.render_complete_semaphore = render_complete_fence;
    return true;
}

bool api::present_curr_frame()
{
    auto rhi = get_rhi();

    // query the current frame (also update the frame index)
    auto& frame = rhi->current_frame();

    // query the semaphores
    auto& image_available_fence = fetch_resource(rhi->fences, frame.image_available_semaphore);
    auto& render_complete_fence = fetch_resource(rhi->fences, frame.render_complete_semaphore);

    // check if nothing has been down, insert dummy workloads if needed
    if (frame.allocated_command_buffers.empty()) {
        auto  command_buffer_handle = frame.allocate(GPUQueueType::COMPUTE, true);
        auto& command_buffer        = frame.allocated_command_buffers.at(command_buffer_handle.value);
        command_buffer.begin();
        default_swapchain_image_barrier(command_buffer.command_buffer);
        command_buffer.end();
        command_buffer.wait(image_available_fence, GPUBarrierSync::NONE);
        command_buffer.signal(render_complete_fence, GPUBarrierSync::ALL);
        command_buffer.submit();
    }

    // present to swapchain
    auto present_info               = VkPresentInfoKHR{};
    present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext              = nullptr;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &render_complete_fence.semaphore;
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &rhi->swapchain;
    present_info.pImageIndices      = &rhi->current_image_index;
    present_info.pResults           = nullptr;

    VkResult result = rhi->vtable.vkQueuePresentKHR(rhi->present_queue, &present_info);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
        // recreate the swapchain if window resizes or moved to other displays
        api::wait_idle();
        rhi->create_swapchain();
        return true;
    }

    vk_check(result);
    rhi->current_frame_index++;
    return true;
}
