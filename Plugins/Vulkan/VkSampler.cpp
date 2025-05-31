#include "VkUtils.h"

// NOTE: A possible optimization is that we could hash the sampler descriptor
// to avoid creating identical samplers because many of the times the sampler
// is the same.

void VulkanSampler::destroy()
{
    delete_sampler(*this);
}

bool create_sampler(GPUSamplerHandle& handle, const GPUSamplerDescriptor& desc)
{
    auto object = create_sampler(desc);

    auto rhi = get_rhi();
    auto ind = rhi->samplers.add(object);

    handle = GPUSamplerHandle(ind);
    return true;
}

void delete_sampler(GPUSamplerHandle handle)
{
    get_rhi()->samplers.remove(handle.value);
}

VulkanSampler create_sampler(const GPUSamplerDescriptor& desc)
{
    auto rhi = get_rhi();

    auto create_info                    = VkSamplerCreateInfo{};
    create_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.anisotropyEnable        = desc.max_anisotropy > 1;
    create_info.unnormalizedCoordinates = false;
    create_info.minLod                  = desc.lod_min_clamp;
    create_info.maxLod                  = desc.lod_max_clamp;
    create_info.compareEnable           = desc.compare_enable;
    create_info.compareOp               = vkenum(desc.compare);
    create_info.minFilter               = vkenum(desc.min_filter);
    create_info.magFilter               = vkenum(desc.mag_filter);
    create_info.mipmapMode              = vkenum(desc.mipmap_filter);
    create_info.addressModeU            = vkenum(desc.address_mode_u);
    create_info.addressModeV            = vkenum(desc.address_mode_v);
    create_info.addressModeW            = vkenum(desc.address_mode_w);
    create_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    create_info.pNext                   = nullptr;
    create_info.flags                   = 0;

    VulkanSampler object;
    vk_check(rhi->vtable.vkCreateSampler(rhi->device, &create_info, nullptr, &object.sampler));
    return object;
}

void delete_sampler(VulkanSampler& sampler)
{
    if (sampler.sampler != VK_NULL_HANDLE) {
        auto rhi = get_rhi();
        rhi->vtable.vkDestroySampler(rhi->device, sampler.sampler, nullptr);
        sampler.sampler = VK_NULL_HANDLE;
    }
}
