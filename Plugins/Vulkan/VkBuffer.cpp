#include "VkUtils.h"

VulkanBuffer::VulkanBuffer()
    : buffer(VK_NULL_HANDLE), alloc_info({})
{
    // do nothing
}

VulkanBuffer::VulkanBuffer(const GPUBufferDescriptor& desc)
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
    vk_check(vmaCreateBuffer(
        get_rhi()->alloc, &buffer_create_info, &alloc_create_info,
        &buffer, &allocation, &alloc_info));

    if (desc.mapped_at_creation) {
        mapped_data = reinterpret_cast<uint8_t*>(alloc_info.pMappedData);
        mapped_size = desc.size;
    }
}

void VulkanBuffer::map(GPUSize64 offset, GPUSize64 size)
{
    void* data = nullptr;
    vk_check(vmaMapMemory(get_rhi()->alloc, allocation, &data));
    mapped_data = reinterpret_cast<uint8_t*>(data) + offset;
    mapped_size = size;
}

void VulkanBuffer::unmap()
{
    vmaUnmapMemory(get_rhi()->alloc, allocation);
}

void VulkanBuffer::destroy()
{
    if (buffer != VK_NULL_HANDLE) {
        vmaDestroyBuffer(get_rhi()->alloc, buffer, allocation);
        buffer = VK_NULL_HANDLE;
    }
}
