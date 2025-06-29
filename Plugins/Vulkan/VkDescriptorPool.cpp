#include "VkUtils.h"

constexpr uint MAX_SETS = 512;

struct DescriptorObjects
{
    List<VkDescriptorBufferInfo> buffers;
    List<VkDescriptorImageInfo>  images;
};

void VulkanDescriptorPool::destroy()
{
    reset();
    for (auto& pool : pools)
        delete_descriptor_pool(pool);
}

void VulkanDescriptorPool::reset()
{
    poolindex = 0;

    allocated.clear();

    for (auto& count : counts)
        count = 0;

    for (auto& pool : pools)
        reset_descriptor_pool(pool);
}

GPUBindGroupHandle VulkanDescriptorPool::allocate(VkDescriptorSet& descriptor, VkDescriptorSetLayout layout, uint set_count, uint bindless_count)
{
    auto rhi = get_rhi();

    poolindex = find_pool_index(poolindex);

    auto alloc_info               = VkDescriptorSetAllocateInfo{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool     = pools.at(poolindex);
    alloc_info.descriptorSetCount = set_count;
    alloc_info.pSetLayouts        = &layout;

    VkDescriptorSetVariableDescriptorCountAllocateInfo set_counts;
    if (bindless_count) {
        set_counts.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
        set_counts.descriptorSetCount = 1;
        set_counts.pDescriptorCounts  = &bindless_count;
        set_counts.pNext              = nullptr;
        alloc_info.pNext              = &set_counts;
    }

    vk_check(rhi->vtable.vkAllocateDescriptorSets(rhi->device, &alloc_info, &descriptor));
    counts.at(poolindex)++;

    uint handle = static_cast<uint>(allocated.size());
    allocated.push_back(descriptor);
    return GPUBindGroupHandle(handle);
}

uint VulkanDescriptorPool::find_pool_index(uint index)
{
    // check if we need to allocate new pool
    if (index >= counts.size()) {
        // allocate a new descriptor pool
        VkDescriptorPool pool = create_descriptor_pool();
        pools.push_back(pool);
        counts.push_back(0);
        return static_cast<uint>(pools.size() - 1);
    }

    if (counts.at(index) < MAX_SETS)
        return index;

    return find_pool_index(index + 1);
}

void fill_descriptor_write(VkWriteDescriptorSet& write, DescriptorObjects& objects, VkDescriptorSet descriptor, const VulkanBindGroupLayout& layout, const GPUBindGroupEntry& entry, uint index)
{
    auto rhi = get_rhi();

    write                  = {};
    write.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorCount  = 1;
    write.descriptorType   = layout.binding_types.at(entry.binding);
    write.dstArrayElement  = index;
    write.dstBinding       = entry.binding;
    write.dstSet           = descriptor;
    write.pBufferInfo      = nullptr;
    write.pImageInfo       = nullptr;
    write.pTexelBufferView = nullptr;
    write.pNext            = nullptr;

    switch (entry.type) {
        case GPUBindingResourceType::BUFFER:
        {
            objects.buffers.emplace_front();
            auto& buffer      = objects.buffers.front();
            buffer.buffer     = fetch_resource(rhi->buffers, entry.buffer.buffer).buffer;
            buffer.offset     = entry.buffer.offset;
            buffer.range      = entry.buffer.size == 0 ? VK_WHOLE_SIZE : entry.buffer.size;
            write.pBufferInfo = &buffer;
            break;
        }
        case GPUBindingResourceType::SAMPLER:
        {
            objects.images.emplace_front();
            auto& sampler       = objects.images.front();
            sampler.imageView   = VK_NULL_HANDLE;
            sampler.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            sampler.sampler     = fetch_resource(rhi->samplers, entry.sampler).sampler;
            write.pImageInfo    = &sampler;
            break;
        }
        case GPUBindingResourceType::TEXTURE:
        {
            objects.images.emplace_front();
            auto& image       = objects.images.front();
            image.imageView   = fetch_resource(rhi->views, entry.texture).view;
            image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image.sampler     = VK_NULL_HANDLE;
            write.pImageInfo  = &image;
            break;
        }
        case GPUBindingResourceType::STORAGE_TEXTURE:
        {
            objects.images.emplace_front();
            auto& image       = objects.images.front();
            image.imageView   = fetch_resource(rhi->views, entry.texture).view;
            image.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            image.sampler     = VK_NULL_HANDLE;
            write.pImageInfo  = &image;
            break;
        }
        case GPUBindingResourceType::ACCELERATION_STRUCTURE:
            assert(!!!"BVH not supported yet!");
            break;
    }
}

GPUBindGroupHandle create_bind_group(const GPUBindGroupDescriptor& desc)
{
    auto  rhi    = get_rhi();
    auto& frame  = rhi->current_frame();
    auto  layout = fetch_resource(rhi->bind_group_layouts, desc.layout);

    // allocate descriptor set
    VkDescriptorSet    descriptor;
    GPUBindGroupHandle handle = frame.descriptor_pool.allocate(descriptor, layout.layout, 1, 0);

    // prepare descriptor writes
    DescriptorObjects            objects;
    Vector<VkWriteDescriptorSet> writes;
    uint                         i = 0;
    for (auto& entry : desc.entries) {
        writes.push_back(VkWriteDescriptorSet{});
        fill_descriptor_write(writes.back(), objects, descriptor, layout, entry, i++);
    }

    // update descriptor sets
    rhi->vtable.vkUpdateDescriptorSets(rhi->device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    return handle;
}

VkDescriptorPool create_descriptor_pool()
{
    auto rhi = get_rhi();

    // clang-format off
    static HashMap<VkDescriptorType, float> allocations = {
        { VK_DESCRIPTOR_TYPE_SAMPLER,                1.0f },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1.0f },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.0f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1.0f },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.0f },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         2.0f },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 2.0f },
    };
    // clang-format on

    Vector<VkDescriptorPoolSize> pool_sizes;
    for (const auto& kv : allocations) {
        auto pool_size            = VkDescriptorPoolSize{};
        pool_size.type            = kv.first;
        pool_size.descriptorCount = static_cast<uint32_t>(MAX_SETS * kv.second);
        pool_sizes.push_back(pool_size);
    }

    // allocate a new descriptor pool
    auto create_info          = VkDescriptorPoolCreateInfo{};
    create_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.poolSizeCount = (uint)pool_sizes.size();
    create_info.pPoolSizes    = pool_sizes.data();
    create_info.maxSets       = MAX_SETS;
    create_info.flags         = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

    VkDescriptorPool pool;
    vk_check(rhi->vtable.vkCreateDescriptorPool(rhi->device, &create_info, nullptr, &pool));
    return pool;
}

void reset_descriptor_pool(VkDescriptorPool pool)
{
    auto rhi = get_rhi();
    vk_check(rhi->vtable.vkResetDescriptorPool(rhi->device, pool, VkDescriptorPoolResetFlags(0)));
}

void delete_descriptor_pool(VkDescriptorPool pool)
{
    auto rhi = get_rhi();
    rhi->vtable.vkDestroyDescriptorPool(rhi->device, pool, nullptr);
}
