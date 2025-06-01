#include "VkUtils.h"

void VulkanShader::destroy()
{
    delete_shader_module(*this);
}

bool create_shader_module(GPUShaderModuleHandle& handle, const GPUShaderModuleDescriptor& desc)
{
    auto obj = create_shader_module(desc);
    auto rhi = get_rhi();
    auto ind = rhi->shaders.add(obj);

    handle = GPUShaderModuleHandle(ind);
    return true;
}

void delete_shader_module(GPUShaderModuleHandle handle)
{
    auto rhi = get_rhi();
    delete_shader_module(fetch_resource(rhi->shaders, handle));
    rhi->shaders.remove(handle.value);
}

VulkanShader create_shader_module(const GPUShaderModuleDescriptor& desc)
{
    auto rhi = get_rhi();

    auto create_info     = VkShaderModuleCreateInfo{};
    create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pCode    = (const uint32_t*)(desc.data);
    create_info.codeSize = desc.size;

    VulkanShader object;
    vk_check(rhi->vtable.vkCreateShaderModule(rhi->device, &create_info, nullptr, &object.module));
    return object;
}

void delete_shader_module(VulkanShader& shader_module)
{
    if (shader_module.module != VK_NULL_HANDLE) {
        auto rhi = get_rhi();
        rhi->vtable.vkDestroyShaderModule(rhi->device, shader_module.module, nullptr);
        shader_module.module = VK_NULL_HANDLE;
    }
}
