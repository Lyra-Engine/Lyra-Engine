#include "VkUtils.h"

void VulkanBuffer::destroy()
{
    delete_buffer(*this);
}

bool create_buffer(GPUBufferHandle& handle, const GPUBufferDescriptor& desc)
{
    auto object = create_buffer(desc);

    auto rhi = get_rhi();
    auto ind = rhi->buffers.add(object);

    handle = GPUBufferHandle(ind);
    return true;
}

void delete_buffer(GPUBufferHandle handle)
{
    get_rhi()->buffers.remove(handle.value);
}

VulkanBuffer create_buffer(const GPUBufferDescriptor& desc)
{
    VkBufferCreateInfo      buffer_create_info = {};
    VmaAllocationCreateInfo alloc_create_info  = {};

    // buffer create info
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size  = desc.size;
    buffer_create_info.usage = vkenum(desc.usage);

    // allocation create info
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_create_info.flags = 0;

    bool dedicate_memory = true;

    if (desc.mapped_at_creation) {
        alloc_create_info.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        dedicate_memory = false;
    }

    if (desc.usage.contains(GPUBufferUsage::MAP_READ)) {
        alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        alloc_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        dedicate_memory = false;
    }

    if (desc.usage.contains(GPUBufferUsage::MAP_WRITE)) {
        alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        alloc_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        dedicate_memory = false;
    }

    if (desc.usage.contains(GPUBufferUsage::UNIFORM)) {
        alloc_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
        alloc_create_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        dedicate_memory = false;
    }

    if (dedicate_memory) {
        alloc_create_info.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    }

    // create buffer
    VulkanBuffer buffer;
    vk_check(vmaCreateBuffer(
        get_rhi()->alloc, &buffer_create_info, &alloc_create_info,
        &buffer.buffer, &buffer.allocation, &buffer.alloc_info));
    return buffer;
}

void delete_buffer(VulkanBuffer& buffer)
{
    if (buffer.buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(get_rhi()->alloc, buffer.buffer, buffer.allocation);
        buffer.buffer = VK_NULL_HANDLE;
    }
}
