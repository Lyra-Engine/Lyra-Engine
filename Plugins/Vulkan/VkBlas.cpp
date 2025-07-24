#include "VkUtils.h"

VulkanBlas::VulkanBlas() : blas(VK_NULL_HANDLE)
{
    // do nothing
}

VulkanBlas::VulkanBlas(const GPUBlasDescriptor& desc, GPUBlasGeometrySizeDescriptors sizes)
{
    auto rhi = get_rhi();

    ranges.clear();
    geometries.clear();
    uint32_t max_primitive_count = 0;

    update_mode = desc.update_mode;

    for (auto& size : sizes) {
        ranges.push_back({});
        geometries.push_back({});

        // define geometry data
        auto& geometry        = geometries.back();
        geometry.sType        = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.pNext        = nullptr;
        geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        geometry.flags        = VK_GEOMETRY_OPAQUE_BIT_KHR;

        // triangle geometry data
        geometry.geometry.triangles.sType                  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        geometry.geometry.triangles.vertexData.hostAddress = nullptr;
        geometry.geometry.triangles.vertexStride           = sizeof(float) * 3; // TODO: assuming float3 vertices
        geometry.geometry.triangles.vertexFormat           = vkenum(size.triangles.vertex_format);
        geometry.geometry.triangles.maxVertex              = size.triangles.vertex_count - 1;
        geometry.geometry.triangles.indexData.hostAddress  = nullptr;
        geometry.geometry.triangles.indexType              = vkenum(size.triangles.index_format);

        // triangle build range
        auto& range           = ranges.back();
        range.primitiveCount  = size.triangles.index_count / 3;
        range.primitiveOffset = 0;
        range.firstVertex     = 0;
        range.transformOffset = 0;

        // update max primitive count
        max_primitive_count = std::max(max_primitive_count, range.primitiveOffset);
    }

    // configure build info
    build.sType         = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    build.type          = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    build.flags         = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    build.mode          = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    build.geometryCount = static_cast<uint32_t>(geometries.size());
    build.pGeometries   = geometries.data();
    build.flags         = vkenum(desc.flags);

    // get size requirements
    this->sizes       = VkAccelerationStructureBuildSizesInfoKHR{};
    this->sizes.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    this->sizes.pNext = nullptr;
    rhi->vtable.vkGetAccelerationStructureBuildSizesKHR(rhi->device,
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &build, &max_primitive_count, &this->sizes);

    // create buffer to store BLAS
    auto additional             = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
    auto buffer_desc            = GPUBufferDescriptor{};
    buffer_desc.usage           = 0;
    buffer_desc.size            = this->sizes.accelerationStructureSize;
    buffer_desc.virtual_address = true;
    storage                     = VulkanBuffer(buffer_desc, additional);

    // create BLAS vulkan object
    auto create_info   = VkAccelerationStructureCreateInfoKHR{};
    create_info.sType  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    create_info.buffer = storage.buffer;
    create_info.size   = this->sizes.accelerationStructureSize;
    create_info.type   = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    auto result = rhi->vtable.vkCreateAccelerationStructureKHR(rhi->device, &create_info, nullptr, &blas);
    if (result != VK_SUCCESS) destroy();
    vk_check(result);

    auto address_info                  = VkAccelerationStructureDeviceAddressInfoKHR{};
    address_info.sType                 = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    address_info.accelerationStructure = blas;
    vk_check(rhi->vtable.vkGetAccelerationStructureDeviceAddressKHR(rhi->device, &address_info));

    if (desc.label)
        rhi->set_debug_label(VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, (uint64_t)blas, desc.label);
}

void VulkanBlas::destroy()
{
    auto rhi = get_rhi();

    storage.destroy();

    if (blas != VK_NULL_HANDLE) {
        rhi->vtable.vkDestroyAccelerationStructureKHR(rhi->device, blas, nullptr);
        blas = VK_NULL_HANDLE;
    }
}
