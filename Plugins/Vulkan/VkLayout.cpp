#include "VkUtils.h"

// NOTE: A possible optimization is that we could hash the layout
// to avoid creating identical layouts, not sure if this is useful.

void VulkanBindGroupLayout::destroy()
{
    delete_bind_group_layout(*this);
}

void VulkanPipelineLayout::destroy()
{
    delete_pipeline_layout(*this);
}

bool create_bind_group_layout(GPUBindGroupLayoutHandle& handle, const GPUBindGroupLayoutDescriptor& desc)
{
    auto obj = create_bind_group_layout(desc);
    auto rhi = get_rhi();
    auto ind = rhi->bind_group_layouts.add(obj);

    handle = GPUBindGroupLayoutHandle(ind);
    return true;
}

void delete_bind_group_layout(GPUBindGroupLayoutHandle handle)
{
    auto rhi = get_rhi();
    delete_bind_group_layout(fetch_resource(rhi->bind_group_layouts, handle));
    rhi->bind_group_layouts.remove(handle.value);
}

VulkanBindGroupLayout create_bind_group_layout(const GPUBindGroupLayoutDescriptor& desc)
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

    VulkanBindGroupLayout layout;
    layout.layout = VK_NULL_HANDLE;

    // nothing in the descriptor set
    if (bindings.size() == 0)
        return layout;

    // prepare create info
    auto create_info         = VkDescriptorSetLayoutCreateInfo{};
    create_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.pBindings    = bindings.data();
    create_info.bindingCount = bindings.size();
    create_info.pNext        = &bindingflags_info;

    // create descritpor set layout
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkCreateDescriptorSetLayout(rhi->device, &create_info, nullptr, &layout.layout));
    return layout;
}

void delete_bind_group_layout(VulkanBindGroupLayout& layout)
{
    auto rhi = get_rhi();
    rhi->vtable.vkDestroyDescriptorSetLayout(rhi->device, layout.layout, nullptr);
    layout.layout = VK_NULL_HANDLE;
}

bool create_pipeline_layout(GPUPipelineLayoutHandle& handle, const GPUPipelineLayoutDescriptor& desc)
{
    auto obj = create_pipeline_layout(desc);
    auto rhi = get_rhi();
    auto ind = rhi->pipeline_layouts.add(obj);

    handle = GPUPipelineLayoutHandle(ind);
    return true;
}

void delete_pipeline_layout(GPUPipelineLayoutHandle handle)
{
    auto rhi = get_rhi();
    delete_pipeline_layout(fetch_resource(rhi->pipeline_layouts, handle));
    rhi->pipeline_layouts.remove(handle.value);
}

VulkanPipelineLayout create_pipeline_layout(const GPUPipelineLayoutDescriptor& desc)
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
    VulkanPipelineLayout layout;
    vk_check(rhi->vtable.vkCreatePipelineLayout(rhi->device, &create_info, nullptr, &layout.layout));
    return layout;
}

void delete_pipeline_layout(VulkanPipelineLayout& layout)
{
    auto rhi = get_rhi();
    rhi->vtable.vkDestroyPipelineLayout(rhi->device, layout.layout, nullptr);
    layout.layout = VK_NULL_HANDLE;
}
