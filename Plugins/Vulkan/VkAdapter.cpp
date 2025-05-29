#include "VkUtils.h"

bool create_adapter(GPUAdapter& adapter, const GPUAdapterDescriptor& descriptor)
{
    auto rhi = get_rhi();

    uint count;
    vk_check(vkEnumeratePhysicalDevices(rhi->instance, &count, nullptr));

    Vector<VkPhysicalDevice> devices(count);
    vk_check(vkEnumeratePhysicalDevices(rhi->instance, &count, devices.data()));

    // TODO: actually pick the most suitable adapter.
    rhi->adapter = devices.at(0);

    // TODO: populate adapter info, features and limits
    return true;
}

void delete_adapter()
{
    // Vulkan Physical Device does not need to be deleted
}
