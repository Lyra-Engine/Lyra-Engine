#include "VkUtils.h"

VkCommandPool create_command_pool(uint queue_family_index)
{
    auto rhi = get_rhi();

    auto pool_info             = VkCommandPoolCreateInfo{};
    pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.pNext            = NULL;
    pool_info.flags            = 0;
    pool_info.queueFamilyIndex = queue_family_index;

    VkCommandPool pool;
    vk_check(rhi->vtable.vkCreateCommandPool(rhi->device, &pool_info, nullptr, &pool));
    return pool;
}

void delete_command_pool(VkCommandPool pool)
{
    auto rhi = get_rhi();
    rhi->vtable.vkDestroyCommandPool(rhi->device, pool, nullptr);
}

void reset_command_pool(VkCommandPool pool)
{
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkResetCommandPool(rhi->device, pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
}

VkCommandBuffer allocate_command_buffer(VkCommandPool pool, bool primary)
{
    auto rhi = get_rhi();

    auto alloc_info               = VkCommandBufferAllocateInfo{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext              = nullptr;
    alloc_info.commandPool        = pool,
    alloc_info.level              = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vk_check(rhi->vtable.vkAllocateCommandBuffers(rhi->device, &alloc_info, &command_buffer));
    return command_buffer;
}
