#include "D3D12Utils.h"

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
