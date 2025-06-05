#include "VkUtils.h"

// NOTE: A possible optimization is that we could hash the layout
// to avoid creating identical layouts, not sure if this is useful.

VulkanBindGroupLayout::VulkanBindGroupLayout() : layout(VK_NULL_HANDLE)
{
    // do nothing
}

VulkanBindGroupLayout::VulkanBindGroupLayout(const GPUBindGroupLayoutDescriptor& desc)
{
    VkDescriptorBindingFlags flags[] = {VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT};

    auto bindingflags_info          = VkDescriptorSetLayoutBindingFlagsCreateInfo{};
    bindingflags_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingflags_info.bindingCount  = 1;
    bindingflags_info.pBindingFlags = nullptr;

    // extract binding information for the descriptor set
    Vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto& entry : desc.entries) {
        auto binding               = VkDescriptorSetLayoutBinding{};
        binding.binding            = entry.binding;
        binding.descriptorCount    = entry.count;
        binding.descriptorType     = vkenum(entry.type);
        binding.stageFlags         = vkenum(entry.visibility);
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);

        // variable size descriptor binding
        if (is_binding_array(entry.type))
            bindingflags_info.pBindingFlags = flags;
    }

    layout = VK_NULL_HANDLE;

    if (!bindings.empty()) {
        // prepare create info
        auto create_info         = VkDescriptorSetLayoutCreateInfo{};
        create_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        create_info.pBindings    = bindings.data();
        create_info.bindingCount = bindings.size();
        create_info.pNext        = &bindingflags_info;

        // create descritpor set layout
        auto rhi = get_rhi();
        vk_check(rhi->vtable.vkCreateDescriptorSetLayout(rhi->device, &create_info, nullptr, &layout));
    }
}

void VulkanBindGroupLayout::destroy()
{
    auto rhi = get_rhi();
    rhi->vtable.vkDestroyDescriptorSetLayout(rhi->device, layout, nullptr);
    layout = VK_NULL_HANDLE;
}

VulkanPipelineLayout::VulkanPipelineLayout() : layout(VK_NULL_HANDLE)
{
    // do nothing
}

VulkanPipelineLayout::VulkanPipelineLayout(const GPUPipelineLayoutDescriptor& desc)
{
    auto rhi = get_rhi();

    Vector<VkDescriptorSetLayout> bind_group_layouts;
    for (const auto& handle : desc.bind_group_layouts) {
        auto& bind_group_layout = rhi->bind_group_layouts.data.at(handle.value);
        assert(bind_group_layout.layout != VK_NULL_HANDLE);
        bind_group_layouts.push_back(bind_group_layout.layout);
    }

    // prepare pipeline layout
    auto create_info           = VkPipelineLayoutCreateInfo{};
    create_info.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.pSetLayouts    = bind_group_layouts.data();
    create_info.setLayoutCount = static_cast<uint32_t>(bind_group_layouts.size());

    // create pipeline layout
    vk_check(rhi->vtable.vkCreatePipelineLayout(rhi->device, &create_info, nullptr, &layout));
}

void VulkanPipelineLayout::destroy()
{
    auto rhi = get_rhi();
    rhi->vtable.vkDestroyPipelineLayout(rhi->device, layout, nullptr);
    layout = VK_NULL_HANDLE;
}
