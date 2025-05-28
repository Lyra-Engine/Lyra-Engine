#ifndef LYRA_PLUGIN_VULKAN_VKMAPPINGS_H
#define LYRA_PLUGIN_VULKAN_VKMAPPINGS_H

#include <Render/Render.hpp>

#include "VkInclude.h"

using namespace lyra;
using namespace lyra::rhi;

auto vkenum(GPUPresentMode mode) -> VkPresentModeKHR;
auto vkenum(GPUCompositeAlphaMode mode) -> VkCompositeAlphaFlagsKHR;
auto vkenum(GPUColorSpace space) -> VkColorSpaceKHR;
auto vkenum(GPUBlendOperation op) -> VkBlendOp;
auto vkenum(GPUBlendFactor factor) -> VkBlendFactor;
auto vkenum(GPULoadOp op) -> VkAttachmentLoadOp;
auto vkenum(GPUStoreOp op) -> VkAttachmentStoreOp;
auto vkenum(GPUQueryType query) -> VkQueryType;
auto vkenum(GPUTextureAspect aspect) -> VkImageAspectFlags;
auto vkenum(GPUTextureDimension dim) -> VkImageType;
auto vkenum(GPUTextureViewDimension dim) -> VkImageViewType;
auto vkenum(GPUAddressMode mode) -> VkSamplerAddressMode;
auto vkenum(GPUFilterMode filter) -> VkFilter;
auto vkenum(GPUMipmapFilterMode filter) -> VkSamplerMipmapMode;
auto vkenum(GPUCompareFunction op) -> VkCompareOp;
auto vkenum(GPUStencilOperation op) -> VkStencilOp;
auto vkenum(GPUFrontFace winding) -> VkFrontFace;
auto vkenum(GPUCullMode culling) -> VkCullModeFlagBits;
auto vkenum(GPUPrimitiveTopology topology) -> VkPrimitiveTopology;
auto vkenum(GPUVertexStepMode step) -> VkVertexInputRate;
auto vkenum(GPUIndexFormat format) -> VkIndexType;
auto vkenum(GPUVertexFormat format) -> VkFormat;
auto vkenum(GPUTextureFormat format) -> VkFormat;
auto vkenum(GPUBarrierLayout layout) -> VkImageLayout;
auto vkenum(GPUColorWriteFlags color) -> VkColorComponentFlags;
auto vkenum(GPUBufferUsageFlags usages) -> VkBufferUsageFlags;
auto vkenum(GPUTextureUsageFlags usages) -> VkImageUsageFlags;
auto vkenum(GPUShaderStageFlags stages) -> VkShaderStageFlags;
auto vkenum(GPUBarrierSyncFlags flags) -> VkPipelineStageFlags;
auto vkenum(GPUBarrierAccessFlags flags) -> VkAccessFlagBits;

#endif // LYRA_PLUGIN_VULKAN_VKMAPPINGS_H
