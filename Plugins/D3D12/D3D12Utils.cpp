#include "D3D12Utils.h"
#include "d3d12.h"
#include "dxgiformat.h"

static Logger logger = init_stderr_logger("D3D12", LogLevel::trace);

static D3D12RHI* D3D12_RHI = nullptr;

void set_rhi(D3D12RHI* instance)
{
    D3D12_RHI = instance;
}

auto get_rhi() -> D3D12RHI*
{
    return D3D12_RHI;
}

Logger get_logger()
{
    return logger;
}

D3D12_HEAP_TYPE infer_heap_type(GPUBufferUsageFlags usages)
{
    D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT;

    if (usages.contains(GPUBufferUsage::MAP_READ))
        return D3D12_HEAP_TYPE_READBACK;

    if (usages.contains(GPUBufferUsage::MAP_WRITE))
        return D3D12_HEAP_TYPE_UPLOAD;

    return type;
}

D3D12_RESOURCE_FLAGS infer_buffer_flags(GPUBufferUsageFlags usages)
{
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

    if (usages.contains(GPUBufferUsage::STORAGE)) {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    return flags;
}

D3D12_RESOURCE_FLAGS infer_texture_flags(GPUTextureUsageFlags usages, GPUTextureFormat format)
{
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

    if (usages.contains(GPUTextureUsage::RENDER_ATTACHMENT)) {
        if (is_depth_format(format) || is_stencil_format(format)) {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        } else {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }
    }

    if (usages.contains(GPUTextureUsage::TEXTURE_BINDING)) {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    return flags;
}

DXGI_FORMAT infer_texture_format(GPUTextureFormat format)
{
    switch (format) {
        case GPUTextureFormat::R8UNORM:
            return DXGI_FORMAT_R8_UNORM;
        case GPUTextureFormat::R8SNORM:
            return DXGI_FORMAT_R8_SNORM;
        case GPUTextureFormat::R8UINT:
            return DXGI_FORMAT_R8_UINT;
        case GPUTextureFormat::R8SINT:
            return DXGI_FORMAT_R8_SINT;
        case GPUTextureFormat::R16UNORM:
            return DXGI_FORMAT_R16_UNORM;
        case GPUTextureFormat::R16SNORM:
            return DXGI_FORMAT_R16_SNORM;
        case GPUTextureFormat::R16UINT:
            return DXGI_FORMAT_R16_UINT;
        case GPUTextureFormat::R16SINT:
            return DXGI_FORMAT_R16_SINT;
        case GPUTextureFormat::RG8UNORM:
            return DXGI_FORMAT_R8G8_UNORM;
        case GPUTextureFormat::RG8SNORM:
            return DXGI_FORMAT_R8G8_SNORM;
        case GPUTextureFormat::RG8UINT:
            return DXGI_FORMAT_R8G8_UINT;
        case GPUTextureFormat::RG8SINT:
            return DXGI_FORMAT_R8G8_SINT;
        case GPUTextureFormat::R32UINT:
            return DXGI_FORMAT_R32_UINT;
        case GPUTextureFormat::R32SINT:
            return DXGI_FORMAT_R32_SINT;
        case GPUTextureFormat::R32FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case GPUTextureFormat::RG16UNORM:
            return DXGI_FORMAT_R16G16_UNORM;
        case GPUTextureFormat::RG16SNORM:
            return DXGI_FORMAT_R16G16_SNORM;
        case GPUTextureFormat::RG16UINT:
            return DXGI_FORMAT_R16G16_UINT;
        case GPUTextureFormat::RG16SINT:
            return DXGI_FORMAT_R16G16_SINT;
        case GPUTextureFormat::RG16FLOAT:
            return DXGI_FORMAT_R16G16_FLOAT;
        case GPUTextureFormat::RGBA8UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case GPUTextureFormat::RGBA8UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case GPUTextureFormat::RGBA8SNORM:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
        case GPUTextureFormat::RGBA8UINT:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case GPUTextureFormat::RGBA8SINT:
            return DXGI_FORMAT_R8G8B8A8_SINT;
        case GPUTextureFormat::BGRA8UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case GPUTextureFormat::BGRA8UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case GPUTextureFormat::RGB9E5UFLOAT:
            return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        case GPUTextureFormat::RGB10A2UINT:
            return DXGI_FORMAT_R10G10B10A2_UINT;
        case GPUTextureFormat::RGB10A2UNORM:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case GPUTextureFormat::RG11B10UFLOAT: // TODO: Check if this mapping is correct.
            return DXGI_FORMAT_R10G10B10A2_TYPELESS;
        case GPUTextureFormat::RG32UINT:
            return DXGI_FORMAT_R32G32_UINT;
        case GPUTextureFormat::RG32SINT:
            return DXGI_FORMAT_R32G32_SINT;
        case GPUTextureFormat::RG32FLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
        case GPUTextureFormat::RGBA16UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case GPUTextureFormat::RGBA16SNORM:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case GPUTextureFormat::RGBA16UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case GPUTextureFormat::RGBA16SINT:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case GPUTextureFormat::RGBA16FLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case GPUTextureFormat::RGBA32UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case GPUTextureFormat::RGBA32SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case GPUTextureFormat::RGBA32FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case GPUTextureFormat::STENCIL8: // TODO: Check if this mapping is correct.
            return DXGI_FORMAT_R24G8_TYPELESS;
        case GPUTextureFormat::DEPTH16UNORM:
            return DXGI_FORMAT_D16_UNORM;
        case GPUTextureFormat::DEPTH24PLUS:
            return DXGI_FORMAT_D24_UNORM_S8_UINT; // TODO: Check if this mapping is correct.
        case GPUTextureFormat::DEPTH24PLUS_STENCIL8:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case GPUTextureFormat::DEPTH32FLOAT:
            return DXGI_FORMAT_D32_FLOAT;
        case GPUTextureFormat::DEPTH32FLOAT_STENCIL8:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case GPUTextureFormat::BC1_RGBA_UNORM:
            return DXGI_FORMAT_BC1_UNORM;
        case GPUTextureFormat::BC1_RGBA_UNORM_SRGB:
            return DXGI_FORMAT_BC1_UNORM_SRGB;
        case GPUTextureFormat::BC2_RGBA_UNORM:
            return DXGI_FORMAT_BC2_UNORM;
        case GPUTextureFormat::BC2_RGBA_UNORM_SRGB:
            return DXGI_FORMAT_BC2_UNORM_SRGB;
        case GPUTextureFormat::BC3_RGBA_UNORM:
            return DXGI_FORMAT_BC3_UNORM;
        case GPUTextureFormat::BC3_RGBA_UNORM_SRGB:
            return DXGI_FORMAT_BC3_UNORM_SRGB;
        case GPUTextureFormat::BC4_R_UNORM:
            return DXGI_FORMAT_BC4_UNORM;
        case GPUTextureFormat::BC4_R_SNORM:
            return DXGI_FORMAT_BC4_SNORM;
        case GPUTextureFormat::BC5_RG_UNORM:
            return DXGI_FORMAT_BC5_UNORM;
        case GPUTextureFormat::BC5_RG_SNORM:
            return DXGI_FORMAT_BC5_SNORM;
        case GPUTextureFormat::BC6H_RGB_UFLOAT:
            return DXGI_FORMAT_BC6H_UF16;
        case GPUTextureFormat::BC6H_RGB_FLOAT:
            return DXGI_FORMAT_BC6H_SF16;
        case GPUTextureFormat::BC7_RGBA_UNORM:
            return DXGI_FORMAT_BC7_UNORM;
        case GPUTextureFormat::BC7_RGBA_UNORM_SRGB:
            return DXGI_FORMAT_BC7_UNORM_SRGB;
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
            assert(!!!"D3D12 does not support EAC formats!");
            return DXGI_FORMAT_UNKNOWN;
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
            assert(!!!"D3D12 does not support ASTC formats!");
            return DXGI_FORMAT_UNKNOWN;
    }
    throw std::invalid_argument("invalid argument for GPUTextureFormat");
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE infer_topology_type(GPUPrimitiveTopology topology)
{
    switch (topology) {
        case GPUPrimitiveTopology::POINT_LIST:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case GPUPrimitiveTopology::LINE_LIST:
        case GPUPrimitiveTopology::LINE_STRIP:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case GPUPrimitiveTopology::TRIANGLE_LIST:
        case GPUPrimitiveTopology::TRIANGLE_STRIP:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    }
}

D3D12_PRIMITIVE_TOPOLOGY infer_topology(GPUPrimitiveTopology topology)
{
    switch (topology) {
        case GPUPrimitiveTopology::POINT_LIST:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case GPUPrimitiveTopology::LINE_LIST:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case GPUPrimitiveTopology::LINE_STRIP:
            return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case GPUPrimitiveTopology::TRIANGLE_LIST:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case GPUPrimitiveTopology::TRIANGLE_STRIP:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
}

D3D12_COMPARISON_FUNC d3d12enum(GPUCompareFunction compare, bool enable)
{
    if (!enable)
        return D3D12_COMPARISON_FUNC_ALWAYS;

    switch (compare) {
        case GPUCompareFunction::NEVER:
            return D3D12_COMPARISON_FUNC_NEVER;
        case GPUCompareFunction::LESS:
            return D3D12_COMPARISON_FUNC_LESS;
        case GPUCompareFunction::EQUAL:
            return D3D12_COMPARISON_FUNC_EQUAL;
        case GPUCompareFunction::LESS_EQUAL:
            return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case GPUCompareFunction::GREATER:
            return D3D12_COMPARISON_FUNC_GREATER;
        case GPUCompareFunction::NOT_EQUAL:
            return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case GPUCompareFunction::GREATER_EQUAL:
            return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case GPUCompareFunction::ALWAYS:
            return D3D12_COMPARISON_FUNC_ALWAYS;
        default:
            return D3D12_COMPARISON_FUNC_ALWAYS;
    }
}

D3D12_FILTER d3d12enum(GPUFilterMode min, GPUFilterMode mag, GPUMipmapFilterMode mip)
{
    bool min_linear = (min == GPUFilterMode::LINEAR);
    bool mag_linear = (mag == GPUFilterMode::LINEAR);
    bool mip_linear = (mip == GPUMipmapFilterMode::LINEAR);

    if (!min_linear && !mag_linear && !mip_linear)
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    else if (!min_linear && !mag_linear && mip_linear)
        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    else if (!min_linear && mag_linear && !mip_linear)
        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    else if (!min_linear && mag_linear && mip_linear)
        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    else if (min_linear && !mag_linear && !mip_linear)
        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    else if (min_linear && !mag_linear && mip_linear)
        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    else if (min_linear && mag_linear && !mip_linear)
        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    else // min_linear && mag_linear && mip_linear
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
}

D3D12_TEXTURE_ADDRESS_MODE d3d12enum(GPUAddressMode mode)
{
    switch (mode) {
        case GPUAddressMode::REPEAT:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case GPUAddressMode::MIRROR_REPEAT:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case GPUAddressMode::CLAMP_TO_EDGE:
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    }
    throw std::invalid_argument("invalid argument for GPUAddressMode");
}

DXGI_FORMAT d3d12enum(GPUVertexFormat format)
{
    switch (format) {
        case GPUVertexFormat::UINT8:
            return DXGI_FORMAT_R8_UINT;
        case GPUVertexFormat::UINT8x2:
            return DXGI_FORMAT_R8G8_UINT;
        case GPUVertexFormat::UINT8x4:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case GPUVertexFormat::SINT8:
            return DXGI_FORMAT_R8_SINT;
        case GPUVertexFormat::SINT8x2:
            return DXGI_FORMAT_R8G8_SINT;
        case GPUVertexFormat::SINT8x4:
            return DXGI_FORMAT_R8G8B8A8_SINT;
        case GPUVertexFormat::UNORM8:
            return DXGI_FORMAT_R8_UNORM;
        case GPUVertexFormat::UNORM8x2:
            return DXGI_FORMAT_R8G8_UNORM;
        case GPUVertexFormat::UNORM8x4:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case GPUVertexFormat::SNORM8:
            return DXGI_FORMAT_R8_SNORM;
        case GPUVertexFormat::SNORM8x2:
            return DXGI_FORMAT_R8G8_SNORM;
        case GPUVertexFormat::SNORM8x4:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
        case GPUVertexFormat::UINT16:
            return DXGI_FORMAT_R16_UINT;
        case GPUVertexFormat::UINT16x2:
            return DXGI_FORMAT_R16G16_UINT;
        case GPUVertexFormat::UINT16x4:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case GPUVertexFormat::SINT16:
            return DXGI_FORMAT_R16_SINT;
        case GPUVertexFormat::SINT16x2:
            return DXGI_FORMAT_R16G16_SINT;
        case GPUVertexFormat::SINT16x4:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case GPUVertexFormat::UNORM16:
            return DXGI_FORMAT_R16_UNORM;
        case GPUVertexFormat::UNORM16x2:
            return DXGI_FORMAT_R16G16_UNORM;
        case GPUVertexFormat::UNORM16x4:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case GPUVertexFormat::SNORM16:
            return DXGI_FORMAT_R16_SNORM;
        case GPUVertexFormat::SNORM16x2:
            return DXGI_FORMAT_R16G16_SNORM;
        case GPUVertexFormat::SNORM16x4:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case GPUVertexFormat::FLOAT16:
            return DXGI_FORMAT_R16_FLOAT;
        case GPUVertexFormat::FLOAT16x2:
            return DXGI_FORMAT_R16G16_FLOAT;
        case GPUVertexFormat::FLOAT16x4:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case GPUVertexFormat::FLOAT32:
            return DXGI_FORMAT_R32_FLOAT;
        case GPUVertexFormat::FLOAT32x2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case GPUVertexFormat::FLOAT32x3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case GPUVertexFormat::FLOAT32x4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case GPUVertexFormat::UINT32:
            return DXGI_FORMAT_R32_UINT;
        case GPUVertexFormat::UINT32x2:
            return DXGI_FORMAT_R32G32_UINT;
        case GPUVertexFormat::UINT32x3:
            return DXGI_FORMAT_R32G32B32_UINT;
        case GPUVertexFormat::UINT32x4:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case GPUVertexFormat::SINT32:
            return DXGI_FORMAT_R32_SINT;
        case GPUVertexFormat::SINT32x2:
            return DXGI_FORMAT_R32G32_SINT;
        case GPUVertexFormat::SINT32x3:
            return DXGI_FORMAT_R32G32B32_SINT;
        case GPUVertexFormat::SINT32x4:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        default:
            return DXGI_FORMAT_UNKNOWN;
    }
}

D3D12_INPUT_CLASSIFICATION d3d12enum(GPUVertexStepMode stepMode)
{
    switch (stepMode) {
        case GPUVertexStepMode::VERTEX:
            return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        case GPUVertexStepMode::INSTANCE:
            return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        default:
            return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    }
}

D3D12_CULL_MODE d3d12enum(GPUCullMode cull)
{
    switch (cull) {
        case GPUCullMode::NONE:
            return D3D12_CULL_MODE_NONE;
        case GPUCullMode::FRONT:
            return D3D12_CULL_MODE_FRONT;
        case GPUCullMode::BACK:
            return D3D12_CULL_MODE_BACK;
        default:
            return D3D12_CULL_MODE_BACK;
    }
}

D3D12_STENCIL_OP d3d12enum(GPUStencilOperation op)
{
    switch (op) {
        case GPUStencilOperation::KEEP:
            return D3D12_STENCIL_OP_KEEP;
        case GPUStencilOperation::ZERO:
            return D3D12_STENCIL_OP_ZERO;
        case GPUStencilOperation::REPLACE:
            return D3D12_STENCIL_OP_REPLACE;
        case GPUStencilOperation::INVERT:
            return D3D12_STENCIL_OP_INVERT;
        case GPUStencilOperation::INCREMENT_CLAMP:
            return D3D12_STENCIL_OP_INCR_SAT;
        case GPUStencilOperation::DECREMENT_CLAMP:
            return D3D12_STENCIL_OP_DECR_SAT;
        case GPUStencilOperation::INCREMENT_WRAP:
            return D3D12_STENCIL_OP_INCR;
        case GPUStencilOperation::DECREMENT_WRAP:
            return D3D12_STENCIL_OP_DECR;
        default:
            return D3D12_STENCIL_OP_KEEP;
    }
}

D3D12_BLEND_OP d3d12enum(GPUBlendOperation op)
{
    switch (op) {
        case GPUBlendOperation::ADD:
            return D3D12_BLEND_OP_ADD;
        case GPUBlendOperation::SUBTRACT:
            return D3D12_BLEND_OP_SUBTRACT;
        case GPUBlendOperation::REVERSE_SUBTRACT:
            return D3D12_BLEND_OP_REV_SUBTRACT;
        case GPUBlendOperation::MIN:
            return D3D12_BLEND_OP_MIN;
        case GPUBlendOperation::MAX:
            return D3D12_BLEND_OP_MAX;
        default:
            return D3D12_BLEND_OP_ADD;
    }
}

D3D12_BLEND d3d12enum(GPUBlendFactor factor)
{
    switch (factor) {
        case GPUBlendFactor::ZERO:
            return D3D12_BLEND_ZERO;
        case GPUBlendFactor::ONE:
            return D3D12_BLEND_ONE;
        case GPUBlendFactor::SRC:
            return D3D12_BLEND_SRC_COLOR;
        case GPUBlendFactor::ONE_MINUS_SRC:
            return D3D12_BLEND_INV_SRC_COLOR;
        case GPUBlendFactor::SRC_ALPHA:
            return D3D12_BLEND_SRC_ALPHA;
        case GPUBlendFactor::ONE_MINUS_SRC_ALPHA:
            return D3D12_BLEND_INV_SRC_ALPHA;
        case GPUBlendFactor::DST:
            return D3D12_BLEND_DEST_COLOR;
        case GPUBlendFactor::ONE_MINUS_DST:
            return D3D12_BLEND_INV_DEST_COLOR;
        case GPUBlendFactor::DST_ALPHA:
            return D3D12_BLEND_DEST_ALPHA;
        case GPUBlendFactor::ONE_MINUS_DST_ALPHA:
            return D3D12_BLEND_INV_DEST_ALPHA;
        case GPUBlendFactor::SRC_ALPHA_SATURATED:
            return D3D12_BLEND_SRC_ALPHA_SAT;
        case GPUBlendFactor::CONSTANT:
            return D3D12_BLEND_BLEND_FACTOR;
        case GPUBlendFactor::ONE_MINUS_CONSTANT:
            return D3D12_BLEND_INV_BLEND_FACTOR;
        default:
            return D3D12_BLEND_ONE;
    }
}

DXGI_FORMAT d3d12enum(GPUIndexFormat format)
{
    switch (format) {
        case GPUIndexFormat::UINT16:
            return DXGI_FORMAT_R16_UINT;
        case GPUIndexFormat::UINT32:
            return DXGI_FORMAT_R32_UINT;
        default:
            return DXGI_FORMAT_R32_UINT;
    }
}
