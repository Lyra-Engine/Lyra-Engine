#include "VkUtils.h"

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
    int width, height;
    Window::api()->get_window_size(desc.window, width, height);

    VkExtent2D actual_extent;
    actual_extent.width  = width;
    actual_extent.height = height;
    actual_extent.width  = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return actual_extent;
}

// vulkan swapchain utils
bool acquire_next_frame(GPUTextureHandle& texture, GPUFenceHandle& image_available_fence, GPUFenceHandle& render_complete_fence, bool& suboptimal)
{
    auto rhi = get_rhi();

    // query the current frame
    auto& frame    = rhi->current_frame();
    frame.frame_id = rhi->current_frame_index;

    // wait for inflght frame to complete
    frame.wait();
    frame.reset();

    // acquire next frame
    auto semaphore = fetch_resource(rhi->fences, frame.image_available_semaphore);
    auto result    = rhi->vtable.vkAcquireNextImageKHR(rhi->device, rhi->swapchain, UINT64_MAX, semaphore.semaphore, VK_NULL_HANDLE, &rhi->current_image_index);
    if (result == VK_SUBOPTIMAL_KHR) {
        suboptimal = true;
    } else {
        vk_check(result);
    }

    // update fences
    image_available_fence = frame.image_available_semaphore;
    render_complete_fence = frame.render_complete_semaphore;
    return true;
}

// NOTE: This is only a temporary solution.
// We will switch to regular pipeline barriers later.
void temporary_solution_swapchain_image_barrier(VkCommandBuffer command_buffer)
{
    auto rhi = get_rhi();

    auto handle  = rhi->images.at(rhi->current_image_index);
    auto texture = fetch_resource(rhi->textures, handle);

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

bool present_curr_frame(GPUTextureHandle texture)
{
    auto rhi = get_rhi();

    // query the current frame (also update the frame index)
    auto& frame    = rhi->current_frame();
    frame.frame_id = rhi->current_frame_index;

    // query the semaphores
    auto& image_available_fence = fetch_resource(rhi->fences, frame.image_available_semaphore);
    auto& render_complete_fence = fetch_resource(rhi->fences, frame.render_complete_semaphore);

    // check if nothing has been down, insert dummy workloads if needed
    if (frame.allocated_command_buffers.empty()) {
        auto  command_buffer_handle = frame.allocate(GPUQueueType::COMPUTE, true);
        auto& command_buffer        = frame.allocated_command_buffers.at(command_buffer_handle.value);
        command_buffer.begin();
        temporary_solution_swapchain_image_barrier(command_buffer.command_buffer);
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

    vk_check(rhi->vtable.vkQueuePresentKHR(rhi->present_queue, &present_info));

    rhi->current_frame_index++;
    return true;
}
