#include <Lyra/Common/Logger.h>
#include <Lyra/Common/Stdint.h>
#include <Lyra/Assets/Builtin/GenericAsset.h>
#include <Lyra/Assets/Builtin/TextureAsset.h>

#include <ktx.h>
#include <vulkan/vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "KtxAsset.h"

using namespace lyra;

static Logger logger = lyra::init_stdout_logger("ktx", LogLevel::trace);

static Logger get_logger()
{
    return logger;
}

static GPUTextureFormat to_gpu_texture_format(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return GPUTextureFormat::RGBA8UNORM;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return GPUTextureFormat::RGBA32FLOAT;
        default:
            return GPUTextureFormat::RGBA8UNORM;
    }
}

static void* load_ktx_asset(FileLoader* loader, const JSON& metadata)
{
    auto path    = metadata["path"].template get<String>();
    auto content = loader->read<uint8_t>(path.c_str());

    ktxTexture2*   ktx_texture;
    KTX_error_code result = ktxTexture2_CreateFromMemory(content.data(), content.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_texture);

    if (result != KTX_SUCCESS) {
        get_logger()->error("Failed to create KTX texture from memory: {}", ktxErrorString(result));
        return nullptr;
    }

    auto texture_asset    = new TextureAsset();
    texture_asset->format = to_gpu_texture_format(static_cast<VkFormat>(ktx_texture->vkFormat));
    texture_asset->binary.resize(ktx_texture->dataSize);
    memcpy(texture_asset->binary.data(), ktx_texture->pData, ktx_texture->dataSize);

    for (uint32_t i = 0; i < ktx_texture->numLevels; ++i) {
        ktx_size_t offset;
        ktxTexture_GetImageOffset(ktxTexture(ktx_texture), i, 0, 0, &offset);
        texture_asset->subresources.push_back({static_cast<uint32_t>(offset), ktxTexture_GetRowPitch(reinterpret_cast<ktxTexture*>(ktx_texture), i), ktx_texture->baseWidth >> i, ktx_texture->baseHeight >> i});
    }

    ktxTexture_Destroy(ktxTexture(ktx_texture));
    return texture_asset;
}

static void unload_texture_asset(void* asset)
{
    delete reinterpret_cast<TextureAsset*>(asset);
}

static JSON process_ktx_asset(AssetServer* manager, OSPath source_path, OSPath target_path)
{
    auto source_path_cstr = reinterpret_cast<const char*>(source_path);

    int    width, height, channels;
    void*  pixels;
    bool   is_hdr     = stbi_is_hdr(source_path_cstr);
    size_t pixel_size = 0;

    if (is_hdr) {
        pixels     = stbi_loadf(source_path_cstr, &width, &height, &channels, STBI_rgb_alpha);
        pixel_size = sizeof(float);
    } else {
        pixels     = stbi_load(source_path_cstr, &width, &height, &channels, STBI_rgb_alpha);
        pixel_size = sizeof(stbi_uc);
    }

    if (!pixels) {
        get_logger()->error("Failed to load image file: {}", reinterpret_cast<const char*>(source_path));
        return {};
    }

    ktxTexture2*         texture;
    ktxTextureCreateInfo createInfo;
    createInfo.glInternalformat = 0;
    createInfo.vkFormat         = is_hdr ? VK_FORMAT_R32G32B32A32_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.baseWidth        = width;
    createInfo.baseHeight       = height;
    createInfo.baseDepth        = 1;
    createInfo.numDimensions    = 2;
    createInfo.numLevels        = 1;
    createInfo.numLayers        = 1;
    createInfo.numFaces         = 1;
    createInfo.isArray          = KTX_FALSE;
    createInfo.generateMipmaps  = KTX_TRUE;

    KTX_error_code result = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture);
    if (result != KTX_SUCCESS) {
        get_logger()->error("Failed to create KTX2 texture: {}", ktxErrorString(result));
        stbi_image_free(pixels);
        return {};
    }

    result = ktxTexture_SetImageFromMemory(ktxTexture(texture), 0, 0, 0, static_cast<ktx_uint8_t*>(pixels), width * height * 4 * pixel_size);
    if (result != KTX_SUCCESS) {
        get_logger()->error("Failed to set image data for KTX2 texture: {}", ktxErrorString(result));
        ktxTexture_Destroy(ktxTexture(texture));
        stbi_image_free(pixels);
        return {};
    }

    result = ktxTexture_WriteToNamedFile(ktxTexture(texture), reinterpret_cast<const char*>(target_path));
    if (result != KTX_SUCCESS) {
        get_logger()->error("Failed to write KTX2 texture to file: {}", ktxErrorString(result));
        ktxTexture_Destroy(ktxTexture(texture));
        stbi_image_free(pixels);
        return {};
    }

    ktxTexture_Destroy(ktxTexture(texture));
    stbi_image_free(pixels);

    auto metadata    = JSON{};
    metadata["path"] = reinterpret_cast<const char*>(target_path);
    return metadata;
}

AssetHandlerAPI KtxAsset::handler()
{
    auto api    = AssetHandlerAPI{};
    api.load    = load_ktx_asset;
    api.unload  = unload_texture_asset;
    api.process = process_ktx_asset;
    return api;
}
