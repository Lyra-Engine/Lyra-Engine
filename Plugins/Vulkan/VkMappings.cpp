#include "VkUtils.h"

VkPresentModeKHR vkenum(GPUPresentMode mode)
{
    switch (mode) {
        case GPUPresentMode::Fifo:
            return VK_PRESENT_MODE_FIFO_KHR;
        case GPUPresentMode::FifoRelaxed:
            return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        case GPUPresentMode::Immediate:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case GPUPresentMode::Mailbox:
            return VK_PRESENT_MODE_MAILBOX_KHR;
    }
}

VkCompositeAlphaFlagsKHR vkenum(GPUCompositeAlphaMode mode)
{
    switch (mode) {
        case GPUCompositeAlphaMode::Opaque:
            return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        case GPUCompositeAlphaMode::PostMultiplied:
            return VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        case GPUCompositeAlphaMode::PreMultiplied:
            return VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    }
}

VkColorSpaceKHR vkenum(GPUColorSpace space)
{
    switch (space) {
        case GPUColorSpace::SRGB:
            return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        case GPUColorSpace::DISPLAY_P3:
            return VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT;
    }
}

VkBlendOp vkenum(GPUBlendOperation op)
{
    switch (op) {
        case GPUBlendOperation::ADD:
            return VK_BLEND_OP_ADD;
        case GPUBlendOperation::SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;
        case GPUBlendOperation::REVERSE_SUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case GPUBlendOperation::MIN:
            return VK_BLEND_OP_MIN;
        case GPUBlendOperation::MAX:
            return VK_BLEND_OP_MAX;
    }
}

VkBlendFactor vkenum(GPUBlendFactor factor)
{
    switch (factor) {
        case GPUBlendFactor::ZERO:
            return VK_BLEND_FACTOR_ZERO;
        case GPUBlendFactor::ONE:
            return VK_BLEND_FACTOR_ONE;
        case GPUBlendFactor::SRC:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case GPUBlendFactor::ONE_MINUS_SRC:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case GPUBlendFactor::SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case GPUBlendFactor::ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case GPUBlendFactor::DST:
            return VK_BLEND_FACTOR_DST_COLOR;
        case GPUBlendFactor::ONE_MINUS_DST:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case GPUBlendFactor::DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case GPUBlendFactor::ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case GPUBlendFactor::SRC_ALPHA_SATURATED:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case GPUBlendFactor::CONSTANT:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case GPUBlendFactor::ONE_MINUS_CONSTANT:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case GPUBlendFactor::SRC1:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case GPUBlendFactor::ONE_MINUS_SRC1:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case GPUBlendFactor::SRC1_ALPHA:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case GPUBlendFactor::ONE_MINUS_SRC1_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    }
}

VkAttachmentLoadOp vkenum(GPULoadOp op)
{
    switch (op) {
        case GPULoadOp::CLEAR:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case GPULoadOp::LOAD:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
    }
}

VkAttachmentStoreOp vkenum(GPUStoreOp op)
{
    switch (op) {
        case GPUStoreOp::DISCARD:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        case GPUStoreOp::STORE:
            return VK_ATTACHMENT_STORE_OP_STORE;
    }
}

VkQueryType vkenum(GPUQueryType query)
{
    switch (query) {
        case GPUQueryType::OCCLUSION:
            return VK_QUERY_TYPE_OCCLUSION;
        case GPUQueryType::TIMESTAMP:
            return VK_QUERY_TYPE_TIMESTAMP;
    }
}

VkImageAspectFlags vkenum(GPUTextureAspect aspect)
{
    switch (aspect) {
        case GPUTextureAspect::ALL:
            return VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        case GPUTextureAspect::DEPTH_ONLY:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case GPUTextureAspect::STENCIL_ONLY:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
    }
}

VkImageType vkenum(GPUTextureDimension dim)
{
    switch (dim) {
        case GPUTextureDimension::x1D:
            return VK_IMAGE_TYPE_1D;
        case GPUTextureDimension::x2D:
            return VK_IMAGE_TYPE_2D;
        case GPUTextureDimension::x3D:
            return VK_IMAGE_TYPE_3D;
    }
}

VkImageViewType vkenum(GPUTextureViewDimension dim)
{
    switch (dim) {
        case GPUTextureViewDimension::x1D:
            return VK_IMAGE_VIEW_TYPE_1D;
        case GPUTextureViewDimension::x2D:
            return VK_IMAGE_VIEW_TYPE_2D;
        case GPUTextureViewDimension::x2D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case GPUTextureViewDimension::CUBE:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        case GPUTextureViewDimension::CUBE_ARRAY:
            return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        case GPUTextureViewDimension::x3D:
            return VK_IMAGE_VIEW_TYPE_3D;
    }
}

VkSamplerAddressMode vkenum(GPUAddressMode mode)
{
    switch (mode) {
        case GPUAddressMode::CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case GPUAddressMode::REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case GPUAddressMode::MIRROR_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    }
}

VkFilter vkenum(GPUFilterMode filter)
{
    switch (filter) {
        case GPUFilterMode::NEAREST:
            return VK_FILTER_NEAREST;
        case GPUFilterMode::LINEAR:
            return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode vkenum(GPUMipmapFilterMode filter)
{
    switch (filter) {
        case GPUMipmapFilterMode::NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case GPUMipmapFilterMode::LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

VkCompareOp vkenum(GPUCompareFunction op)
{
    switch (op) {
        case GPUCompareFunction::ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        case GPUCompareFunction::NEVER:
            return VK_COMPARE_OP_NEVER;
        case GPUCompareFunction::GREATER:
            return VK_COMPARE_OP_GREATER;
        case GPUCompareFunction::GREATER_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case GPUCompareFunction::LESS:
            return VK_COMPARE_OP_LESS;
        case GPUCompareFunction::LESS_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case GPUCompareFunction::EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case GPUCompareFunction::NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
    }
}

VkStencilOp vkenum(GPUStencilOperation op)
{
    switch (op) {
        case GPUStencilOperation::KEEP:
            return VK_STENCIL_OP_KEEP;
        case GPUStencilOperation::ZERO:
            return VK_STENCIL_OP_ZERO;
        case GPUStencilOperation::REPLACE:
            return VK_STENCIL_OP_REPLACE;
        case GPUStencilOperation::INVERT:
            return VK_STENCIL_OP_INVERT;
        case GPUStencilOperation::INCREMENT_CLAMP:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case GPUStencilOperation::DECREMENT_CLAMP:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case GPUStencilOperation::INCREMENT_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case GPUStencilOperation::DECREMENT_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }
}

VkFrontFace vkenum(GPUFrontFace winding)
{
    switch (winding) {
        case GPUFrontFace::CW:
            return VK_FRONT_FACE_CLOCKWISE;
        case GPUFrontFace::CCW:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}

VkCullModeFlagBits vkenum(GPUCullMode culling)
{
    switch (culling) {
        case GPUCullMode::NONE:
            return VK_CULL_MODE_NONE;
        case GPUCullMode::BACK:
            return VK_CULL_MODE_BACK_BIT;
        case GPUCullMode::FRONT:
            return VK_CULL_MODE_FRONT_BIT;
    }
}

VkPrimitiveTopology vkenum(GPUPrimitiveTopology topology)
{
    switch (topology) {
        case GPUPrimitiveTopology::POINT_LIST:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case GPUPrimitiveTopology::LINE_LIST:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case GPUPrimitiveTopology::LINE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case GPUPrimitiveTopology::TRIANGLE_LIST:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case GPUPrimitiveTopology::TRIANGLE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    }
}

VkVertexInputRate vkenum(GPUVertexStepMode step)
{
    switch (step) {
        case GPUVertexStepMode::VERTEX:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case GPUVertexStepMode::INSTANCE:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
}

VkIndexType vkenum(GPUIndexFormat format)
{
    switch (format) {
        case GPUIndexFormat::UINT16:
            return VK_INDEX_TYPE_UINT16;
        case GPUIndexFormat::UINT32:
            return VK_INDEX_TYPE_UINT32;
    }
}

VkFormat vkenum(GPUVertexFormat format)
{
    switch (format) {
        case GPUVertexFormat::UINT8:
            return VK_FORMAT_R8_UINT;
        case GPUVertexFormat::UINT8x2:
            return VK_FORMAT_R8G8_UINT;
        case GPUVertexFormat::UINT8x4:
            return VK_FORMAT_R8G8B8A8_UINT;
        case GPUVertexFormat::SINT8:
            return VK_FORMAT_R8_SINT;
        case GPUVertexFormat::SINT8x2:
            return VK_FORMAT_R8G8_SINT;
        case GPUVertexFormat::SINT8x4:
            return VK_FORMAT_R8G8B8A8_SINT;
        case GPUVertexFormat::UNORM8:
            return VK_FORMAT_R8_UNORM;
        case GPUVertexFormat::UNORM8x2:
            return VK_FORMAT_R8G8_UNORM;
        case GPUVertexFormat::UNORM8x4:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case GPUVertexFormat::SNORM8:
            return VK_FORMAT_R8_SNORM;
        case GPUVertexFormat::SNORM8x2:
            return VK_FORMAT_R8G8_SNORM;
        case GPUVertexFormat::SNORM8x4:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case GPUVertexFormat::UINT16:
            return VK_FORMAT_R16_UINT;
        case GPUVertexFormat::UINT16x2:
            return VK_FORMAT_R16G16_UINT;
        case GPUVertexFormat::UINT16x4:
            return VK_FORMAT_R16G16B16A16_UINT;
        case GPUVertexFormat::SINT16:
            return VK_FORMAT_R16_SINT;
        case GPUVertexFormat::SINT16x2:
            return VK_FORMAT_R16G16_SINT;
        case GPUVertexFormat::SINT16x4:
            return VK_FORMAT_R16G16B16A16_SINT;
        case GPUVertexFormat::UNORM16:
            return VK_FORMAT_R16_UNORM;
        case GPUVertexFormat::UNORM16x2:
            return VK_FORMAT_R16G16_UNORM;
        case GPUVertexFormat::UNORM16x4:
            return VK_FORMAT_R16G16B16A16_UNORM;
        case GPUVertexFormat::SNORM16:
            return VK_FORMAT_R16_SNORM;
        case GPUVertexFormat::SNORM16x2:
            return VK_FORMAT_R16G16_SNORM;
        case GPUVertexFormat::SNORM16x4:
            return VK_FORMAT_R16G16B16A16_SNORM;
        case GPUVertexFormat::FLOAT16:
            return VK_FORMAT_R16_SFLOAT;
        case GPUVertexFormat::FLOAT16x2:
            return VK_FORMAT_R16G16_SFLOAT;
        case GPUVertexFormat::FLOAT16x4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case GPUVertexFormat::FLOAT32:
            return VK_FORMAT_R32_SFLOAT;
        case GPUVertexFormat::FLOAT32x2:
            return VK_FORMAT_R32G32_SFLOAT;
        case GPUVertexFormat::FLOAT32x3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case GPUVertexFormat::FLOAT32x4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case GPUVertexFormat::UINT32:
            return VK_FORMAT_R32_UINT;
        case GPUVertexFormat::UINT32x2:
            return VK_FORMAT_R32G32_UINT;
        case GPUVertexFormat::UINT32x3:
            return VK_FORMAT_R32G32B32_UINT;
        case GPUVertexFormat::UINT32x4:
            return VK_FORMAT_R32G32B32A32_UINT;
        case GPUVertexFormat::SINT32:
            return VK_FORMAT_R32_SINT;
        case GPUVertexFormat::SINT32x2:
            return VK_FORMAT_R32G32_SINT;
        case GPUVertexFormat::SINT32x3:
            return VK_FORMAT_R32G32B32_SINT;
        case GPUVertexFormat::SINT32x4:
            return VK_FORMAT_R32G32B32A32_SINT;
        case GPUVertexFormat::UNORM10_10_10_2:
            return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case GPUVertexFormat::UNORM8x4_BGRA:
            return VK_FORMAT_B8G8R8A8_UNORM;
    }
}

VkFormat vkenum(GPUTextureFormat format)
{
    switch (format) {
        case GPUTextureFormat::R8UNORM:
            return VK_FORMAT_R8_UNORM;
        case GPUTextureFormat::R8SNORM:
            return VK_FORMAT_R8_SNORM;
        case GPUTextureFormat::R8UINT:
            return VK_FORMAT_R8_UINT;
        case GPUTextureFormat::R8SINT:
            return VK_FORMAT_R8_SINT;
        case GPUTextureFormat::R16UNORM:
            return VK_FORMAT_R16_UNORM;
        case GPUTextureFormat::R16SNORM:
            return VK_FORMAT_R16_SNORM;
        case GPUTextureFormat::R16UINT:
            return VK_FORMAT_R16_UINT;
        case GPUTextureFormat::R16SINT:
            return VK_FORMAT_R16_SINT;
        case GPUTextureFormat::RG8UNORM:
            return VK_FORMAT_R8G8_UNORM;
        case GPUTextureFormat::RG8SNORM:
            return VK_FORMAT_R8G8_SNORM;
        case GPUTextureFormat::RG8UINT:
            return VK_FORMAT_R8G8_UINT;
        case GPUTextureFormat::RG8SINT:
            return VK_FORMAT_R8G8_SINT;
        case GPUTextureFormat::R32UINT:
            return VK_FORMAT_R32_UINT;
        case GPUTextureFormat::R32SINT:
            return VK_FORMAT_R32_SINT;
        case GPUTextureFormat::R32FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case GPUTextureFormat::RG16UNORM:
            return VK_FORMAT_R16G16_UNORM;
        case GPUTextureFormat::RG16SNORM:
            return VK_FORMAT_R16G16_SNORM;
        case GPUTextureFormat::RG16UINT:
            return VK_FORMAT_R16G16_UINT;
        case GPUTextureFormat::RG16SINT:
            return VK_FORMAT_R16G16_SINT;
        case GPUTextureFormat::RG16FLOAT:
            return VK_FORMAT_R16G16_SFLOAT;
        case GPUTextureFormat::RGBA8UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case GPUTextureFormat::RGBA8UNORM_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case GPUTextureFormat::RGBA8SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case GPUTextureFormat::RGBA8UINT:
            return VK_FORMAT_R8G8B8A8_UINT;
        case GPUTextureFormat::RGBA8SINT:
            return VK_FORMAT_R8G8B8A8_SINT;
        case GPUTextureFormat::BGRA8UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case GPUTextureFormat::BGRA8UNORM_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case GPUTextureFormat::RGB9E5UFLOAT:
            return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case GPUTextureFormat::RGB10A2UINT:
            return VK_FORMAT_A2R10G10B10_UINT_PACK32;
        case GPUTextureFormat::RGB10A2UNORM:
            return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case GPUTextureFormat::RG11B10UFLOAT:
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case GPUTextureFormat::RG32UINT:
            return VK_FORMAT_R32G32_UINT;
        case GPUTextureFormat::RG32SINT:
            return VK_FORMAT_R32G32_SINT;
        case GPUTextureFormat::RG32FLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case GPUTextureFormat::RGBA16UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;
        case GPUTextureFormat::RGBA16SNORM:
            return VK_FORMAT_R16G16B16A16_SNORM;
        case GPUTextureFormat::RGBA16UINT:
            return VK_FORMAT_R16G16B16A16_UINT;
        case GPUTextureFormat::RGBA16SINT:
            return VK_FORMAT_R16G16B16A16_SINT;
        case GPUTextureFormat::RGBA16FLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case GPUTextureFormat::RGBA32UINT:
            return VK_FORMAT_R32G32B32A32_UINT;
        case GPUTextureFormat::RGBA32SINT:
            return VK_FORMAT_R32G32B32A32_SINT;
        case GPUTextureFormat::RGBA32FLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case GPUTextureFormat::STENCIL8:
            return VK_FORMAT_S8_UINT;
        case GPUTextureFormat::DEPTH16UNORM:
            return VK_FORMAT_D16_UNORM;
        case GPUTextureFormat::DEPTH24PLUS:
            return VK_FORMAT_X8_D24_UNORM_PACK32;
        case GPUTextureFormat::DEPTH24PLUS_STENCIL8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case GPUTextureFormat::DEPTH32FLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case GPUTextureFormat::DEPTH32FLOAT_STENCIL8:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case GPUTextureFormat::BC1_RGBA_UNORM:
        case GPUTextureFormat::BC1_RGBA_UNORM_SRGB:
        case GPUTextureFormat::BC2_RGBA_UNORM:
        case GPUTextureFormat::BC2_RGBA_UNORM_SRGB:
        case GPUTextureFormat::BC3_RGBA_UNORM:
        case GPUTextureFormat::BC3_RGBA_UNORM_SRGB:
        case GPUTextureFormat::BC4_R_UNORM:
        case GPUTextureFormat::BC4_R_SNORM:
        case GPUTextureFormat::BC5_RG_UNORM:
        case GPUTextureFormat::BC5_RG_SNORM:
        case GPUTextureFormat::BC6H_RGB_UFLOAT:
        case GPUTextureFormat::BC6H_RGB_FLOAT:
        case GPUTextureFormat::BC7_RGBA_UNORM:
        case GPUTextureFormat::BC7_RGBA_UNORM_SRGB:
            assert(!!!"BC formats are temporarily NOT supported!");
            return VK_FORMAT_UNDEFINED;
        case GPUTextureFormat::ETC2_RGB8UNORM:
        case GPUTextureFormat::ETC2_RGB8UNORM_SRGB:
        case GPUTextureFormat::ETC2_RGB8A1UNORM:
        case GPUTextureFormat::ETC2_RGB8A1UNORM_SRGB:
        case GPUTextureFormat::ETC2_RGBA8UNORM:
        case GPUTextureFormat::ETC2_RGBA8UNORM_SRGB:
        case GPUTextureFormat::EAC_R11UNORM:
        case GPUTextureFormat::EAC_R11SNORM:
        case GPUTextureFormat::EAC_RG11UNORM:
        case GPUTextureFormat::EAC_RG11SNORM:
            assert(!!!"EAC formats are temporarily NOT supported!");
            return VK_FORMAT_UNDEFINED;
        case GPUTextureFormat::ASTC_4X4_UNORM:
        case GPUTextureFormat::ASTC_4X4_UNORM_SRGB:
        case GPUTextureFormat::ASTC_5X4_UNORM:
        case GPUTextureFormat::ASTC_5X4_UNORM_SRGB:
        case GPUTextureFormat::ASTC_5X5_UNORM:
        case GPUTextureFormat::ASTC_5X5_UNORM_SRGB:
        case GPUTextureFormat::ASTC_6X5_UNORM:
        case GPUTextureFormat::ASTC_6X5_UNORM_SRGB:
        case GPUTextureFormat::ASTC_6X6_UNORM:
        case GPUTextureFormat::ASTC_6X6_UNORM_SRGB:
        case GPUTextureFormat::ASTC_8X5_UNORM:
        case GPUTextureFormat::ASTC_8X5_UNORM_SRGB:
        case GPUTextureFormat::ASTC_8X6_UNORM:
        case GPUTextureFormat::ASTC_8X6_UNORM_SRGB:
        case GPUTextureFormat::ASTC_8X8_UNORM:
        case GPUTextureFormat::ASTC_8X8_UNORM_SRGB:
        case GPUTextureFormat::ASTC_10X5_UNORM:
        case GPUTextureFormat::ASTC_10X5_UNORM_SRGB:
        case GPUTextureFormat::ASTC_10X6_UNORM:
        case GPUTextureFormat::ASTC_10X6_UNORM_SRGB:
        case GPUTextureFormat::ASTC_10X8_UNORM:
        case GPUTextureFormat::ASTC_10X8_UNORM_SRGB:
        case GPUTextureFormat::ASTC_10X10_UNORM:
        case GPUTextureFormat::ASTC_10X10_UNORM_SRGB:
        case GPUTextureFormat::ASTC_12X10_UNORM:
        case GPUTextureFormat::ASTC_12X10_UNORM_SRGB:
        case GPUTextureFormat::ASTC_12X12_UNORM:
        case GPUTextureFormat::ASTC_12X12_UNORM_SRGB:
            assert(!!!"ASTC formats are temporarily NOT supported!");
            return VK_FORMAT_UNDEFINED;
    }
}

// VkImageLayout vkenum(GPUBarrierLayout layout)
// {
//     switch (layout) {
//     }
// }

VkColorComponentFlags vkenum(GPUColorWriteFlags color)
{
    VkColorComponentFlags flags = 0;

    // clang-format off
    if (color.contains(GPUColorWrite::RED))   flags |= VK_COLOR_COMPONENT_R_BIT;
    if (color.contains(GPUColorWrite::GREEN)) flags |= VK_COLOR_COMPONENT_G_BIT;
    if (color.contains(GPUColorWrite::BLUE))  flags |= VK_COLOR_COMPONENT_B_BIT;
    if (color.contains(GPUColorWrite::ALPHA)) flags |= VK_COLOR_COMPONENT_A_BIT;
    // clang-format on

    return flags;
}

VkBufferUsageFlags vkenum(GPUBufferUsageFlags usages)
{
    VkBufferUsageFlags flags = 0;

    // clang-format off
    if (usages.contains(GPUBufferUsage::COPY_SRC)) flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    if (usages.contains(GPUBufferUsage::COPY_DST)) flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (usages.contains(GPUBufferUsage::INDEX))    flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (usages.contains(GPUBufferUsage::VERTEX))   flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (usages.contains(GPUBufferUsage::UNIFORM))  flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (usages.contains(GPUBufferUsage::STORAGE))  flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (usages.contains(GPUBufferUsage::INDIRECT)) flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    // clang-format on

    return flags;
}

VkImageUsageFlags vkenum(GPUTextureUsageFlags usages)
{
    VkImageUsageFlags flags = 0;

    // clang-format off
    if (usages.contains(GPUTextureUsage::COPY_SRC))          flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (usages.contains(GPUTextureUsage::COPY_DST))          flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (usages.contains(GPUTextureUsage::TEXTURE_BINDING))   flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (usages.contains(GPUTextureUsage::STORAGE_BINDING))   flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (usages.contains(GPUTextureUsage::RENDER_ATTACHMENT)) flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // clang-format on

    return flags;
}

VkShaderStageFlags vkenum(GPUShaderStageFlags stages)
{
    VkShaderStageFlags flags = 0;

    // clang-format off
    if (stages.contains(GPUShaderStage::VERTEX))    flags |= VK_SHADER_STAGE_VERTEX_BIT;
    if (stages.contains(GPUShaderStage::FRAGMENT))  flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    if (stages.contains(GPUShaderStage::COMPUTE))   flags |= VK_SHADER_STAGE_COMPUTE_BIT;
    if (stages.contains(GPUShaderStage::RAYGEN))    flags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    if (stages.contains(GPUShaderStage::MISS))      flags |= VK_SHADER_STAGE_MISS_BIT_KHR;
    if (stages.contains(GPUShaderStage::CHIT))      flags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    if (stages.contains(GPUShaderStage::AHIT))      flags |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    if (stages.contains(GPUShaderStage::INTERSECT)) flags |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    // clang-format on

    return flags;
}

// VkPipelineStageFlags vkenum(GPUBarrierSyncFlags flags)
// {
// }
//
// VkAccessFlagBits vkenum(GPUBarrierAccessFlags flags)
// {
// }
