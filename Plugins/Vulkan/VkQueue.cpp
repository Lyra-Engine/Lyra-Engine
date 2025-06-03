#include "VkUtils.h"

void wait_idle()
{
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkDeviceWaitIdle(rhi->device));
}

void wait_fence(GPUFenceHandle handle)
{
}
