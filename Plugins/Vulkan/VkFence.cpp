#include "VkUtils.h"

/**
 * TODO: Fences are actually reusable, we don't have to create and destroy all the times,
 * we could reuse them instead.
 **/

void VulkanFence::wait()
{
    wait_timeline_semaphore(*this);
}

void VulkanFence::reset()
{
    reset_timeline_semaphore(*this);
}

bool VulkanFence::ready()
{
    return is_timeline_semaphore_ready(*this);
}

void VulkanFence::destroy()
{
    delete_fence(*this);
}

VulkanFence create_binary_semaphore()
{
    auto rhi = get_rhi();

    auto create_info  = VkSemaphoreCreateInfo{};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;

    VulkanFence fence;
    vk_check(rhi->vtable.vkCreateSemaphore(rhi->device, &create_info, nullptr, &fence.semaphore));
    fence.type   = VK_SEMAPHORE_TYPE_BINARY;
    fence.target = 0;
    return fence;
}

VulkanFence create_timeline_semaphore()
{
    auto rhi = get_rhi();

    auto timeline_create_info          = VkSemaphoreTypeCreateInfo{};
    timeline_create_info.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timeline_create_info.pNext         = nullptr;
    timeline_create_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timeline_create_info.initialValue  = 0;

    auto semaphore_create_info  = VkSemaphoreCreateInfo{};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = &timeline_create_info;
    semaphore_create_info.flags = 0;

    VulkanFence fence;
    vk_check(vkCreateSemaphore(rhi->device, &semaphore_create_info, NULL, &fence.semaphore));
    fence.type   = VK_SEMAPHORE_TYPE_TIMELINE;
    fence.target = 0;
    return fence;
}

void signal_timeline_semaphore(VkSemaphore semaphore, uint64_t value)
{
    auto signal_info      = VkSemaphoreSignalInfo{};
    signal_info.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signal_info.pNext     = nullptr;
    signal_info.semaphore = semaphore;
    signal_info.value     = 5;

    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkSignalSemaphore(rhi->device, &signal_info));
}

void wait_timeline_semaphore(VulkanFence fence, uint64_t timeout)
{
    auto wait_info           = VkSemaphoreWaitInfo{};
    wait_info.sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    wait_info.pNext          = NULL;
    wait_info.flags          = VK_SEMAPHORE_WAIT_ANY_BIT;
    wait_info.semaphoreCount = 1;
    wait_info.pSemaphores    = &fence.semaphore;
    wait_info.pValues        = &fence.target;

    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkWaitSemaphores(rhi->device, &wait_info, 1000000000ULL));

    // NOTE: It could also fail due to VK_TIMEOUT, maybe we want to tackle it.
}

void reset_timeline_semaphore(VulkanFence& fence)
{
    auto rhi = get_rhi();

    uint64_t value;
    vk_check(rhi->vtable.vkGetSemaphoreCounterValue(rhi->device, fence.semaphore, &value));
    fence.target = value + 1;
}

bool is_timeline_semaphore_ready(VulkanFence& fence)
{
    auto rhi = get_rhi();

    uint64_t value;
    vk_check(rhi->vtable.vkGetSemaphoreCounterValue(rhi->device, fence.semaphore, &value));
    return fence.target <= value;
}

bool create_fence(GPUFenceHandle& handle, VkSemaphoreType type)
{
    auto obj = create_fence(type);
    auto rhi = get_rhi();
    auto ind = rhi->fences.add(obj);

    handle = GPUFenceHandle(ind);
    return true;
}

bool create_fence(GPUFenceHandle& handle)
{
    auto obj = create_fence(VK_SEMAPHORE_TYPE_TIMELINE);
    auto rhi = get_rhi();
    auto ind = rhi->fences.add(obj);

    handle = GPUFenceHandle(ind);
    return true;
}

void delete_fence(GPUFenceHandle handle)
{
    auto rhi = get_rhi();
    delete_fence(fetch_resource(rhi->fences, handle));
    rhi->fences.remove(handle.value);
}

VulkanFence create_fence(VkSemaphoreType type)
{
    switch (type) {
        case VK_SEMAPHORE_TYPE_BINARY:
            return create_binary_semaphore();
        default:
            return create_timeline_semaphore();
    }
}

void delete_fence(VulkanFence& fence)
{
    if (fence.semaphore != VK_NULL_HANDLE) {
        auto rhi = get_rhi();
        rhi->vtable.vkDestroySemaphore(rhi->device, fence.semaphore, nullptr);
        fence.semaphore = VK_NULL_HANDLE;
    }
}

VkFence create_vkfence(bool signaled)
{
    auto rhi = get_rhi();

    auto create_info  = VkFenceCreateInfo{};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkFence vkfence = VK_NULL_HANDLE;
    vk_check(rhi->vtable.vkCreateFence(rhi->device, &create_info, nullptr, &vkfence));
    return vkfence;
}

void delete_vkfence(VkFence vkfence)
{
    auto rhi = get_rhi();
    rhi->vtable.vkDestroyFence(rhi->device, vkfence, nullptr);
}

void reset_vkfence(VkFence vkfence)
{
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkResetFences(rhi->device, 1, &vkfence));
}

void wait_vkfence(VkFence vkfence, uint64_t timeout)
{
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkWaitForFences(rhi->device, 1, &vkfence, VK_TRUE, timeout));
}
