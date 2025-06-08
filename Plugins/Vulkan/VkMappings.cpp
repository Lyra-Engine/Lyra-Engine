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
    throw std::invalid_argument("invalid argument for GPUPresentMode");
}

VkCompositeAlphaFlagBitsKHR vkenum(GPUCompositeAlphaMode mode)
{
    switch (mode) {
        case GPUCompositeAlphaMode::Opaque:
            return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        case GPUCompositeAlphaMode::PostMultiplied:
            return VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
        case GPUCompositeAlphaMode::PreMultiplied:
            return VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    }
    throw std::invalid_argument("invalid argument for GPUCompositeAlphaMode");
}

VkColorSpaceKHR vkenum(GPUColorSpace space)
{
    switch (space) {
        case GPUColorSpace::SRGB:
            return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        case GPUColorSpace::DISPLAY_P3:
            return VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT;
    }
    throw std::invalid_argument("invalid argument for GPUColorSpace");
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
    throw std::invalid_argument("invalid argument for GPUBlendOperation");
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
    throw std::invalid_argument("invalid argument for GPUBlendFactor");
}

VkAttachmentLoadOp vkenum(GPULoadOp op)
{
    switch (op) {
        case GPULoadOp::CLEAR:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case GPULoadOp::LOAD:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        default: // fallback for invalid arguments
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    throw std::invalid_argument("invalid argument for GPULoadOp");
}

VkAttachmentStoreOp vkenum(GPUStoreOp op)
{
    switch (op) {
        case GPUStoreOp::DISCARD:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        case GPUStoreOp::STORE:
            return VK_ATTACHMENT_STORE_OP_STORE;
        default: // fallback for invalid arguments
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
    throw std::invalid_argument("invalid argument for GPUStoreOp");
}

VkQueryType vkenum(GPUQueryType query)
{
    switch (query) {
        case GPUQueryType::OCCLUSION:
            return VK_QUERY_TYPE_OCCLUSION;
        case GPUQueryType::TIMESTAMP:
            return VK_QUERY_TYPE_TIMESTAMP;
        default: // fallback for invalid arguments
            return VK_QUERY_TYPE_OCCLUSION;
    }
    throw std::invalid_argument("invalid argument for GPUQueryType");
}

VkImageAspectFlags vkenum(GPUTextureAspect aspect)
{
    switch (aspect) {
        case GPUTextureAspect::ALL:
            return VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        case GPUTextureAspect::COLOR:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case GPUTextureAspect::DEPTH:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case GPUTextureAspect::STENCIL:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    throw std::invalid_argument("invalid argument for GPUTextureAspect");
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
    throw std::invalid_argument("invalid argument for GPUTextureDimension");
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
    throw std::invalid_argument("invalid argument for GPUTextureViewDimension");
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
    throw std::invalid_argument("invalid argument for GPUAddressMode");
}

VkFilter vkenum(GPUFilterMode filter)
{
    switch (filter) {
        case GPUFilterMode::NEAREST:
            return VK_FILTER_NEAREST;
        case GPUFilterMode::LINEAR:
            return VK_FILTER_LINEAR;
    }
    throw std::invalid_argument("invalid argument for GPUFilterMode");
}

VkSamplerMipmapMode vkenum(GPUMipmapFilterMode filter)
{
    switch (filter) {
        case GPUMipmapFilterMode::NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case GPUMipmapFilterMode::LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
    throw std::invalid_argument("invalid argument for GPUMipmapFilterMode");
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
    throw std::invalid_argument("invalid argument for GPUCompareFunction");
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
    throw std::invalid_argument("invalid argument for GPUStencilOperation");
}

VkFrontFace vkenum(GPUFrontFace winding)
{
    switch (winding) {
        case GPUFrontFace::CW:
            return VK_FRONT_FACE_CLOCKWISE;
        case GPUFrontFace::CCW:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
    throw std::invalid_argument("invalid argument for GPUFrontFace");
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
    throw std::invalid_argument("invalid argument for GPUCullMode");
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
    throw std::invalid_argument("invalid argument for GPUPrimitiveTopology");
}

VkVertexInputRate vkenum(GPUVertexStepMode step)
{
    switch (step) {
        case GPUVertexStepMode::VERTEX:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case GPUVertexStepMode::INSTANCE:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
    throw std::invalid_argument("invalid argument for GPUVertexStepMode");
}

VkIndexType vkenum(GPUIndexFormat format)
{
    switch (format) {
        case GPUIndexFormat::UINT16:
            return VK_INDEX_TYPE_UINT16;
        case GPUIndexFormat::UINT32:
            return VK_INDEX_TYPE_UINT32;
    }
    throw std::invalid_argument("invalid argument for GPUIndexFormat");
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
    throw std::invalid_argument("invalid argument for GPUVertexFormat");
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
    throw std::invalid_argument("invalid argument for GPUTextureFormat");
}

#undef GENREIC_READ
VkImageLayout vkenum(GPUBarrierLayout layout)
{
    switch (layout) {
        case GPUBarrierLayout::UNDEFINED:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case GPUBarrierLayout::COMMON:
            return VK_IMAGE_LAYOUT_GENERAL; // NOTE: requires usage to actually decide what layout
        case GPUBarrierLayout::PRESENT:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        case GPUBarrierLayout::RENDER_TARGET:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case GPUBarrierLayout::UNORDERED_ACCESS:
            return VK_IMAGE_LAYOUT_GENERAL;
        case GPUBarrierLayout::GENERIC_READ:
            return VK_IMAGE_LAYOUT_GENERAL;
        case GPUBarrierLayout::DEPTH_STENCIL_WRITE:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case GPUBarrierLayout::DEPTH_STENCIL_READ:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        case GPUBarrierLayout::SHADER_RESOURCE:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case GPUBarrierLayout::COPY_SOURCE:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case GPUBarrierLayout::COPY_DEST:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case GPUBarrierLayout::RESOLVE_SOURCE:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case GPUBarrierLayout::RESOLVE_DEST:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case GPUBarrierLayout::SHADING_RATE_SOURCE:
            return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
        case GPUBarrierLayout::VIDEO_DECODE_READ:
            return VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR;
        case GPUBarrierLayout::VIDEO_DECODE_WRITE:
            return VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR;
        case GPUBarrierLayout::VIDEO_ENCODE_READ:
            return VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR;
        case GPUBarrierLayout::VIDEO_ENCODE_WRITE:
            return VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR;
        case GPUBarrierLayout::VIDEO_PROCESS_READ:
            return VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR;
        case GPUBarrierLayout::VIDEO_PROCESS_WRITE:
            return VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR;
        case GPUBarrierLayout::DIRECT_QUEUE_COMMON:
        case GPUBarrierLayout::DIRECT_QUEUE_GENERIC_READ:
        case GPUBarrierLayout::DIRECT_QUEUE_UNORDERED_ACCESS:
        case GPUBarrierLayout::DIRECT_QUEUE_SHADER_RESOURCE:
        case GPUBarrierLayout::DIRECT_QUEUE_COPY_SOURCE:
        case GPUBarrierLayout::DIRECT_QUEUE_COPY_DEST:
        case GPUBarrierLayout::COMPUTE_QUEUE_COMMON:
        case GPUBarrierLayout::COMPUTE_QUEUE_GENERIC_READ:
        case GPUBarrierLayout::COMPUTE_QUEUE_UNORDERED_ACCESS:
        case GPUBarrierLayout::COMPUTE_QUEUE_SHADER_RESOURCE:
        case GPUBarrierLayout::COMPUTE_QUEUE_COPY_SOURCE:
        case GPUBarrierLayout::COMPUTE_QUEUE_COPY_DEST:
        case GPUBarrierLayout::VIDEO_QUEUE_COMMON:
            assert(!!!"Unsuported barrier layout in Vulkan!");
            return VK_IMAGE_LAYOUT_GENERAL;
    }
    throw std::invalid_argument("invalid argument for GPUBarrierLayout");
}

VkSampleCountFlagBits vkenum(GPUIntegerCoordinate samples)
{
    switch (samples) {
        case 1:
            return VK_SAMPLE_COUNT_1_BIT;
        case 2:
            return VK_SAMPLE_COUNT_2_BIT;
        case 4:
            return VK_SAMPLE_COUNT_4_BIT;
        case 8:
            return VK_SAMPLE_COUNT_8_BIT;
        default:
            assert(!!!"Non-supported MSAA sample count!");
            return VK_SAMPLE_COUNT_1_BIT;
    }
    throw std::invalid_argument("invalid argument for GPU sample count");
}

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

VkPipelineStageFlags2 vkenum(GPUBarrierSyncFlags sync)
{
    VkPipelineStageFlags flags = 0;

    // clang-format off
    if (sync.contains(GPUBarrierSync::NONE))             flags |= VK_PIPELINE_STAGE_2_NONE;
    if (sync.contains(GPUBarrierSync::ALL))              flags |= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    if (sync.contains(GPUBarrierSync::DRAW))             flags |= VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT; // TODO: figure out the correct mapping for this.
    if (sync.contains(GPUBarrierSync::INDEX_INPUT))      flags |= VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
    if (sync.contains(GPUBarrierSync::VERTEX_SHADING))   flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    if (sync.contains(GPUBarrierSync::PIXEL_SHADING))    flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    if (sync.contains(GPUBarrierSync::DEPTH_STENCIL))    flags |= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
    if (sync.contains(GPUBarrierSync::RENDER_TARGET))    flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    if (sync.contains(GPUBarrierSync::COMPUTE))          flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    if (sync.contains(GPUBarrierSync::RAYTRACING))       flags |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
    if (sync.contains(GPUBarrierSync::COPY))             flags |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    if (sync.contains(GPUBarrierSync::BLIT))             flags |= VK_PIPELINE_STAGE_2_BLIT_BIT;
    if (sync.contains(GPUBarrierSync::CLEAR))            flags |= VK_PIPELINE_STAGE_2_CLEAR_BIT;
    if (sync.contains(GPUBarrierSync::RESOLVE))          flags |= VK_PIPELINE_STAGE_2_RESOLVE_BIT;
    if (sync.contains(GPUBarrierSync::EXECUTE_INDIRECT)) flags |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
    if (sync.contains(GPUBarrierSync::ALL_SHADING))      flags |= VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    if (sync.contains(GPUBarrierSync::VIDEO_DECODE))     flags |= VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR;
    if (sync.contains(GPUBarrierSync::VIDEO_ENCODE))     flags |= VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR;
    if (sync.contains(GPUBarrierSync::BVH_BUILD))        flags |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
    if (sync.contains(GPUBarrierSync::BVH_COPY))         flags |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_COPY_BIT_KHR;
    // clang-format on

    return flags;
}

VkAccessFlags2 vkenum(GPUBarrierAccessFlags access)
{
    VkAccessFlags2 flags = 0;

    // clang-format off
    if (access.contains(GPUBarrierAccess::COMMON))              flags |= VK_ACCESS_2_NONE; // TODO: figure out the correct mapping
    if (access.contains(GPUBarrierAccess::VERTEX_BUFFER))       flags |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
    if (access.contains(GPUBarrierAccess::UNIFORM_BUFFER))      flags |= VK_ACCESS_2_UNIFORM_READ_BIT;
    if (access.contains(GPUBarrierAccess::INDEX_BUFFER))        flags |= VK_ACCESS_2_INDEX_READ_BIT;
    if (access.contains(GPUBarrierAccess::RENDER_TARGET))       flags |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
    if (access.contains(GPUBarrierAccess::UNORDERED_ACCESS))    flags |= VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    if (access.contains(GPUBarrierAccess::DEPTH_STENCIL_WRITE)) flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    if (access.contains(GPUBarrierAccess::DEPTH_STENCIL_READ))  flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    if (access.contains(GPUBarrierAccess::SHADER_RESOURCE))     flags |= VK_ACCESS_2_SHADER_READ_BIT;
    if (access.contains(GPUBarrierAccess::STREAM_OUTPUT))       flags |= VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT;
    if (access.contains(GPUBarrierAccess::INDIRECT_ARGUMENT))   flags |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
    if (access.contains(GPUBarrierAccess::COPY_DEST))           flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
    if (access.contains(GPUBarrierAccess::COPY_SOURCE))         flags |= VK_ACCESS_2_TRANSFER_READ_BIT;
    if (access.contains(GPUBarrierAccess::RESOLVE_DEST))        flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
    if (access.contains(GPUBarrierAccess::RESOLVE_SOURCE))      flags |= VK_ACCESS_2_TRANSFER_READ_BIT;
    if (access.contains(GPUBarrierAccess::BVH_READ))            flags |= VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    if (access.contains(GPUBarrierAccess::BVH_WRITE))           flags |= VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
    if (access.contains(GPUBarrierAccess::SHADING_RATE_SOURCE)) flags |= VK_ACCESS_2_SHADING_RATE_IMAGE_READ_BIT_NV;
    if (access.contains(GPUBarrierAccess::VIDEO_DECODE_READ))   flags |= VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR;
    if (access.contains(GPUBarrierAccess::VIDEO_DECODE_WRITE))  flags |= VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR;
    if (access.contains(GPUBarrierAccess::VIDEO_PROCESS_READ))  flags |= VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR;
    if (access.contains(GPUBarrierAccess::VIDEO_PROCESS_WRITE)) flags |= VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR;
    if (access.contains(GPUBarrierAccess::VIDEO_ENCODE_READ))   flags |= VK_ACCESS_2_VIDEO_ENCODE_READ_BIT_KHR;
    if (access.contains(GPUBarrierAccess::VIDEO_ENCODE_WRITE))  flags |= VK_ACCESS_2_VIDEO_ENCODE_WRITE_BIT_KHR;
    if (access.contains(GPUBarrierAccess::NO_ACCESS))           flags |= VK_ACCESS_2_NONE;
    // clang-format on

    return flags;
}
