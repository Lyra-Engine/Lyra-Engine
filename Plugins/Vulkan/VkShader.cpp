#include "VkUtils.h"

VulkanShader::VulkanShader() : module(VK_NULL_HANDLE)
{
    // do nothing
}

VulkanShader::VulkanShader(const GPUShaderModuleDescriptor& desc)
{
    auto rhi = get_rhi();

    auto create_info     = VkShaderModuleCreateInfo{};
    create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pCode    = (const uint32_t*)(desc.data);
    create_info.codeSize = desc.size;

    vk_check(rhi->vtable.vkCreateShaderModule(rhi->device, &create_info, nullptr, &module));
}

void VulkanShader::destroy()
{
    if (module != VK_NULL_HANDLE) {
        auto rhi = get_rhi();
        rhi->vtable.vkDestroyShaderModule(rhi->device, module, nullptr);
        module = VK_NULL_HANDLE;
    }
}
