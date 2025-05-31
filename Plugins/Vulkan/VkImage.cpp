#include "VkUtils.h"

void VulkanTexture::destroy()
{
    delete_texture(*this);
}

bool create_texture(GPUTextureHandle& handle, const GPUTextureDescriptor& desc)
{
    auto obj = create_texture(desc);
    auto rhi = get_rhi();
    auto ind = rhi->textures.add(obj);

    handle = GPUTextureHandle(ind);
    return true;
}

void delete_texture(GPUTextureHandle handle)
{
    get_rhi()->textures.remove(handle.value);
}

VulkanTexture create_texture(const GPUTextureDescriptor& desc)
{
    VkImageCreateInfo       tex_create_info   = {};
    VmaAllocationCreateInfo alloc_create_info = {};

    // determine image type
    VkImageType image_type;
    if (desc.size.depth > 1) {
        image_type = VK_IMAGE_TYPE_3D;
    } else if (desc.size.height > 1) {
        image_type = VK_IMAGE_TYPE_2D;
    } else {
        image_type = VK_IMAGE_TYPE_1D;
    }

    // buffer create info
    tex_create_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    tex_create_info.extent        = VkExtent3D{desc.size.width, desc.size.height, desc.size.depth};
    tex_create_info.mipLevels     = desc.mip_level_count;
    tex_create_info.arrayLayers   = desc.array_layers;
    tex_create_info.imageType     = image_type;
    tex_create_info.format        = vkenum(desc.format);
    tex_create_info.tiling        = VK_IMAGE_TILING_OPTIMAL;
    tex_create_info.samples       = vkenum(desc.sample_count);
    tex_create_info.usage         = vkenum(desc.usage);
    tex_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // allocation create info
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_create_info.flags = 0;

    // create texture
    VulkanTexture texture;
    vk_check(vmaCreateImage(get_rhi()->alloc,
        &tex_create_info, &alloc_create_info,
        &texture.image, &texture.allocation, &texture.alloc_info));

    // initialize aspects
    if (is_depth_format(desc.format))
        texture.aspects |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if (is_stencil_format(desc.format))
        texture.aspects |= VK_IMAGE_ASPECT_STENCIL_BIT;
    if (texture.aspects == 0)
        texture.aspects |= VK_IMAGE_ASPECT_COLOR_BIT;

    return texture;
}

void delete_texture(VulkanTexture& texture)
{
    if (texture.image != VK_NULL_HANDLE) {
        vmaDestroyImage(get_rhi()->alloc, texture.image, texture.allocation);
        texture.image = VK_NULL_HANDLE;
    }
}
