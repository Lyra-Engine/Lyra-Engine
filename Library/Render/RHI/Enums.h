#ifndef LYRA_LIBRARY_RENDER_RHI_ENUMS_H
#define LYRA_LIBRARY_RENDER_RHI_ENUMS_H

#include <Common/Stdint.h>

namespace lyra::rhi
{
    enum struct GPUBackend : uint
    {
        METAL,
        D3D12,
        VULKAN,
    };

    enum struct GPUFlag : uint
    {
        DEBUG      = 0x1,
        VALIDATION = 0x2,
    };

    enum struct GPUObjectType : uint
    {
        INSTANCE,
        ADAPTER,
        SURFACE,
        DEVICE,
        QUEUE,
        BUFFER,
        SAMPLER,
        TEXTURE,
        TEXTURE_VIEW,
        SHADER_MODULE,
        BINDGROUP,
        BINDGROUP_LAYOUT,
        PIPELINE_LAYOUT,
        COMMAND_BUFFER,
        RENDER_PIPELINE,
        COMPUTE_PIPELINE,
        RAYTRACING_PIPELINE,
        RENDER_BUNDLE,
        QUERY_SET,
        TLAS,
        BLAS,
    };

    enum struct GPUErrorFilter
    {
        VALIDATION,
        OUT_OF_MEMORY,
        INTERNAL,
    };

    enum struct GPUFeatureName : uint
    {
        CORE_FEATURES_AND_LIMITS,
        DEPTH_CLIP_CONTROL,
        DEPTH32FLOAT_STENCIL8,
        TEXTURE_COMPRESSION_BC,
        TEXTURE_COMPRESSION_BC_SLICED_3D,
        TEXTURE_COMPRESSION_ETC2,
        TEXTURE_COMPRESSION_ASTC,
        TEXTURE_COMPRESSION_ASTC_SLICED_3D,
        TIMESTAMP_QUERY,
        INDIRECT_FIRST_INSTANCE,
        SHADER_F16,
        RG11B10UFLOAT_RENDERABLE,
        BGRA8UNORM_STORAGE,
        FLOAT32_FILTERABLE,
        FLOAT32_BLENDABLE,
        CLIP_DISTANCES,
        DUAL_SOURCE_BLENDING,
        SUBGROUPS,
        TEXTURE_FORMATS_TIER1,
    };

    enum struct GPUPresentMode : uint
    {
        Fifo,
        FifoRelaxed,
        Immediate,
        Mailbox,
    };

    enum struct GPUCompositeAlphaMode : uint
    {
        Opaque,
        PreMultiplied,
        PostMultiplied,
    };

    enum struct GPUColorSpace : uint
    {
        SRGB,
        DISPLAY_P3,
    };

    enum struct GPUColorWrite : uint
    {
        RED   = 0x1,
        GREEN = 0x2,
        BLUE  = 0x4,
        ALPHA = 0x8,
        ALL   = 0xF,
    };

    enum struct GPUBlendOperation : uint
    {
        ADD,
        SUBTRACT,
        REVERSE_SUBTRACT,
        MIN,
        MAX,
    };

    enum struct GPUBlendFactor : uint
    {
        ZERO,
        ONE,
        SRC,
        ONE_MINUS_SRC,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST,
        ONE_MINUS_DST,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        SRC_ALPHA_SATURATED,
        CONSTANT,
        ONE_MINUS_CONSTANT,
        SRC1,
        ONE_MINUS_SRC1,
        SRC1_ALPHA,
        ONE_MINUS_SRC1_ALPHA,
    };

    enum struct GPULoadOp : uint
    {
        LOAD,
        CLEAR,
    };

    enum struct GPUStoreOp : uint
    {
        STORE,
        DISCARD,
    };

    enum struct GPUMapMode : uint
    {
        READ  = 0x1,
        WRITE = 0x2,
    };

    enum struct GPUMapState : uint
    {
        UNMAPPED,
        PENDING,
        MAPPED,
    };

    enum struct GPUBufferUsage : uint
    {
        MAP_READ      = 0x0001,
        MAP_WRITE     = 0x0002,
        COPY_SRC      = 0x0004,
        COPY_DST      = 0x0008,
        INDEX         = 0x0010,
        VERTEX        = 0x0020,
        UNIFORM       = 0x0040,
        STORAGE       = 0x0080,
        INDIRECT      = 0x0100,
        QUERY_RESOLVE = 0x0200,
    };

    enum struct GPUTextureUsage : uint
    {
        COPY_SRC          = 0x01,
        COPY_DST          = 0x02,
        TEXTURE_BINDING   = 0x04,
        STORAGE_BINDING   = 0x08,
        RENDER_ATTACHMENT = 0x10,
    };

    enum struct GPUShaderStage : uint
    {
        VERTEX   = 0x1,
        FRAGMENT = 0x2,
        COMPUTE  = 0x4,
    };

    enum struct GPUBindingResourceType : uint
    {
        BUFFER,
        BUFEFR_ARRAY,
        SAMPLER,
        SAMPLER_ARRAY,
        TEXTURE,
        TEXTURE_ARRAY,
        BVH,
    };

    enum struct GPUQueryType : uint
    {
        OCCLUSION,
        TIMESTAMP,
    };

    enum struct GPUBufferBindingType : uint
    {
        UNIFORM,
        STORAGE,
        READ_ONLY_STORAGE,
    };

    enum struct GPUSamplerBindingType : uint
    {
        FILTERING,
        NON_FILTERING,
        COMPARISON,
    };

    enum struct GPUTextureSampleType : uint
    {
        FLOAT,
        UNFILTERABLE_FLOAT,
        DEPTH,
        SINT,
        UINT,
    };

    enum struct GPUStorageTextureAccess : uint
    {
        WRITE_ONLY,
        READ_ONLY,
        READ_WRITE,
    };

    enum struct GPUTextureAspect : uint
    {
        ALL,
        STENCIL_ONLY,
        DEPTH_ONLY,
    };

    enum struct GPUTextureDimension : uint
    {
        x1D,
        x2D,
        x3D,
    };

    enum struct GPUTextureViewDimension : uint
    {
        x1D,
        x2D,
        x2D_ARRAY,
        CUBE,
        CUBE_ARRAY,
        x3D,
    };

    enum struct GPUAddressMode : uint
    {
        CLAMP_TO_EDGE,
        REPEAT,
        MIRROR_REPEAT,
    };

    enum struct GPUFilterMode : uint
    {
        NEAREST,
        LINEAR,
    };

    enum struct GPUMipmapFilterMode : uint
    {
        NEAREST,
        LINEAR,
    };

    enum struct GPUCompareFunction : uint
    {
        NEVER,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_EQUAL,
        ALWAYS,
    };

    enum GPUStencilOperation : uint
    {
        KEEP,
        ZERO,
        REPLACE,
        INVERT,
        INCREMENT_CLAMP,
        DECREMENT_CLAMP,
        INCREMENT_WRAP,
        DECREMENT_WRAP,
    };

    enum struct GPUFrontFace : uint
    {
        CCW,
        CW,
    };

    enum struct GPUCullMode : uint
    {
        NONE,
        FRONT,
        BACK,
    };

    enum struct GPUPrimitiveTopology : uint
    {
        POINT_LIST,
        LINE_LIST,
        LINE_STRIP,
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
    };

    enum struct GPUIndexFormat : uint
    {
        UINT16,
        UINT32,
    };

    enum struct GPUVertexStepMode : uint
    {
        VERTEX,
        INSTANCE,
    };

    enum struct GPUVertexFormat : uint
    {
        UINT8,
        UINT8X2,
        UINT8X4,
        SINT8,
        SINT8X2,
        SINT8X4,
        UNORM8,
        UNORM8X2,
        UNORM8X4,
        SNORM8,
        SNORM8X2,
        SNORM8X4,
        UINT16,
        UINT16X2,
        UINT16X4,
        SINT16,
        SINT16X2,
        SINT16X4,
        UNORM16,
        UNORM16X2,
        UNORM16X4,
        SNORM16,
        SNORM16X2,
        SNORM16X4,
        FLOAT16,
        FLOAT16X2,
        FLOAT16X4,
        FLOAT32,
        FLOAT32X2,
        FLOAT32X3,
        FLOAT32X4,
        UINT32,
        UINT32X2,
        UINT32X3,
        UINT32X4,
        SINT32,
        SINT32X2,
        SINT32X3,
        SINT32X4,
        UNORM10_10_10_2,
        UNORM8X4_BGRA,
    };

    enum struct GPUTextureFormat : uint
    {
        // 8-bit formats
        R8UNORM,
        R8SNORM,
        R8UINT,
        R8SINT,

        // 16-bit formats
        R16UNORM,
        R16SNORM,
        R16UINT,
        R16SINT,
        RG8UNORM,
        RG8SNORM,
        RG8UINT,
        RG8SINT,

        // 32-bit formats
        R32UINT,
        R32SINT,
        R32FLOAT,
        RG16UNORM,
        RG16SNORM,
        RG16UINT,
        RG16SINT,
        RG16FLOAT,
        RGBA8UNORM,
        RGBA8UNORM_SRGB,
        RGBA8SNORM,
        RGBA8UINT,
        RGBA8SINT,
        BGRA8UNORM,
        BGRA8UNORM_SRGB,

        // packed 32-bit formats
        RGB9E5UFLOAT,
        RGB10A2UINT,
        RGB10A2UNORM,
        RG11B10UFLOAT,

        // 64-bit formats
        RG32UINT,
        RG32SINT,
        RG32FLOAT,
        RGBA16UNORM,
        RGBA16SNORM,
        RGBA16UINT,
        RGBA16SINT,
        RGBA16FLOAT,

        // 128-bit formats
        RGBA32UINT,
        RGBA32SINT,
        RGBA32FLOAT,

        // depth/stencil formats
        STENCIL8,
        DEPTH16UNORM,
        DEPTH24PLUS,
        DEPTH24PLUS_STENCIL8,
        DEPTH32FLOAT,

        // depth32float-stencil8 feature
        DEPTH32FLOAT_STENCIL8,

        // BC compressed formats usable if texture-compression-bc is both
        // supported by the device/user agent and enabled in requestDevice.
        BC1_RGBA_UNORM,
        BC1_RGBA_UNORM_SRGB,
        BC2_RGBA_UNORM,
        BC2_RGBA_UNORM_SRGB,
        BC3_RGBA_UNORM,
        BC3_RGBA_UNORM_SRGB,
        BC4_R_UNORM,
        BC4_R_SNORM,
        BC5_RG_UNORM,
        BC5_RG_SNORM,
        BC6H_RGB_UFLOAT,
        BC6H_RGB_FLOAT,
        BC7_RGBA_UNORM,
        BC7_RGBA_UNORM_SRGB,

        // ETC2 compressed formats usable if texture_compression_etc2 is both
        // supported by the device/user agent and enabled in requestDevice.
        ETC2_RGB8UNORM,
        ETC2_RGB8UNORM_SRGB,
        ETC2_RGB8A1UNORM,
        ETC2_RGB8A1UNORM_SRGB,
        ETC2_RGBA8UNORM,
        ETC2_RGBA8UNORM_SRGB,
        EAC_R11UNORM,
        EAC_R11SNORM,
        EAC_RG11UNORM,
        EAC_RG11SNORM,

        // ASTC compressed formats usable if texture_compression_astc is both
        // supported by the device/user agent and enabled in requestDevice.
        ASTC_4X4_UNORM,
        ASTC_4X4_UNORM_SRGB,
        ASTC_5X4_UNORM,
        ASTC_5X4_UNORM_SRGB,
        ASTC_5X5_UNORM,
        ASTC_5X5_UNORM_SRGB,
        ASTC_6X5_UNORM,
        ASTC_6X5_UNORM_SRGB,
        ASTC_6X6_UNORM,
        ASTC_6X6_UNORM_SRGB,
        ASTC_8X5_UNORM,
        ASTC_8X5_UNORM_SRGB,
        ASTC_8X6_UNORM,
        ASTC_8X6_UNORM_SRGB,
        ASTC_8X8_UNORM,
        ASTC_8X8_UNORM_SRGB,
        ASTC_10X5_UNORM,
        ASTC_10X5_UNORM_SRGB,
        ASTC_10X6_UNORM,
        ASTC_10X6_UNORM_SRGB,
        ASTC_10X8_UNORM,
        ASTC_10X8_UNORM_SRGB,
        ASTC_10X10_UNORM,
        ASTC_10X10_UNORM_SRGB,
        ASTC_12X10_UNORM,
        ASTC_12X10_UNORM_SRGB,
        ASTC_12X12_UNORM,
        ASTC_12X12_UNORM_SRGB,
    };

} // namespace lyra::rhi

#endif // LYRA_LIBRARY_RENDER_RHI_ENUMS_H
