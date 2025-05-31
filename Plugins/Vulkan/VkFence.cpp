#include "VkUtils.h"

void VulkanFence::destroy()
{
    delete_fence(*this);
}

bool create_fence(GPUFenceHandle& handle)
{
    auto object = create_fence();

    auto rhi = get_rhi();
    auto ind = rhi->fences.add(object);

    handle = GPUFenceHandle(ind);
    return true;
}

void delete_fence(GPUFenceHandle handle)
{
    get_rhi()->fences.remove(handle.value);
}

VulkanFence create_fence()
{
    auto rhi = get_rhi();

    auto create_info  = VkSemaphoreCreateInfo{};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;

    VulkanFence object;
    vk_check(rhi->vtable.vkCreateSemaphore(rhi->device, &create_info, nullptr, &object.semaphore));
    return object;
}

void delete_fence(VulkanFence& fence)
{
    if (fence.semaphore != VK_NULL_HANDLE) {
        auto rhi = get_rhi();
        rhi->vtable.vkDestroySemaphore(rhi->device, fence.semaphore, nullptr);
        fence.semaphore = VK_NULL_HANDLE;
    }
}
