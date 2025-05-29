#ifndef LYRA_PLUGIN_VULKAN_VKINCLUDE_H
#define LYRA_PLUGIN_VULKAN_VKINCLUDE_H

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef __linux__
#define VK_USE_PLATFORM_X11_KHR
#endif

#ifdef __APPLE__
#define VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_METAL_EXT
#endif

#include <volk.h>
#include <vk_mem_alloc.h>

#include <Common/Logger.h>
#include <Common/Msgbox.h>
#include <Common/Container.h>
#include <Render/Render.hpp>

using namespace lyra;
using namespace lyra::rhi;

// define the Vulkan API version we are going to use
constexpr uint VKAPIVersion = VK_API_VERSION_1_2;

static const char* KHR_PORTABILITY_EXTENSION_NAME = "VK_KHR_portability_subset";
static const char* LUNARG_VALIDATION_LAYER_NAME   = "VK_LAYER_KHRONOS_validation";

struct QueueFamilyIndices
{
    Optional<uint> graphics;
    Optional<uint> compute;
    Optional<uint> present;
    Optional<uint> transfer;
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR   capabilities = {};
    Vector<VkSurfaceFormatKHR> formats;
    Vector<VkPresentModeKHR>   present_modes;
};

struct VulkanBase
{
    VkStructureType sType;
    void*           pNext;
};

struct VulkanRHI
{
    RHIFlags           rhiflags  = 0;
    VkInstance         instance  = VK_NULL_HANDLE;
    VkSurfaceKHR       surface   = VK_NULL_HANDLE;
    VkPhysicalDevice   adapter   = VK_NULL_HANDLE;
    VkDevice           device    = VK_NULL_HANDLE;
    VkSwapchainKHR     swapchain = VK_NULL_HANDLE;
    VkFormat           swapchain_format;
    VkColorSpaceKHR    swapchain_colorspace;
    VolkDeviceTable    vtable;
    VmaAllocator       alloc;
    QueueFamilyIndices queues;
};

struct VulkanBuffer
{
    VkBuffer          buffer = VK_NULL_HANDLE;
    VmaAllocation     allocation;
    VmaAllocationInfo alloc_info;
};

struct VulkanTexture
{
    VkImage           image = VK_NULL_HANDLE;
    VmaAllocation     allocation;
    VmaAllocationInfo alloc_info;
};

struct VulkanFence
{
    VkSemaphore semaphore = VK_NULL_HANDLE;
};

struct VulkanPipeline
{
    VkPipeline       pipeline = VK_NULL_HANDLE;
    VkPipelineLayout layout;
};

auto get_logger() -> Logger;

// to_string
auto to_string(VkResult result) -> CString;
auto to_string(VkDebugUtilsMessageTypeFlagsEXT type) -> CString;
auto to_string(VkDebugUtilsMessageSeverityFlagBitsEXT severity) -> CString;

// enum conversions
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

// vulkan rhi
void set_rhi(VulkanRHI* instance);
auto get_rhi() -> VulkanRHI*;

// vulkan instance
bool create_instance(const RHIDescriptor& desc);
void delete_instance();

// vulkan adapter
bool create_adapter(GPUAdapter& adapter, const GPUAdapterDescriptor& descriptor);
void delete_adapter();

// vulkan surface
void add_surface_extension(Vector<const char*>& instance_extensions);
auto create_surface(VkInstance instance, const WindowHandle& handle) -> VkSurfaceKHR;
bool create_surface(GPUSurface& surface, const GPUSurfaceDescriptor& desc);
void delete_surface();

// vulkan device
bool create_device(const GPUDeviceDescriptor& desc);
void delete_device();

// vulkan buffer
auto create_buffer(const GPUBufferDescriptor& desc) -> VulkanBuffer;
void delete_buffer(VulkanBuffer& buffer);

// vulkan texture
auto create_texture(const GPUTextureDescriptor& desc) -> VulkanTexture;
void delete_texture(VulkanTexture& buffer);

// vulkan fence
auto create_fence() -> VulkanFence;
void delete_fence(VulkanFence& buffer);

// utils
bool has_portability_subset(VkPhysicalDevice physicalDevice);
auto get_supported_instance_extensions() -> HashSet<String>;
auto get_supported_device_extensions(VkPhysicalDevice device) -> HashSet<String>;
auto find_queue_family_indices(VkPhysicalDevice device, VkSurfaceKHR surface) -> QueueFamilyIndices;

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData)
{
    std::stringstream ss;
    ss << "[" << to_string(messageType) << "]";
    ss << "[" << to_string(messageSeverity) << "] ";
    ss << pCallbackData->pMessage;
    get_logger()->warn(ss.str());

    (void)pUserData;
    return VK_FALSE;
}

// helper macro to check vulkan object creation result
#define vk_check(result)                                      \
    {                                                         \
        if (result != VK_SUCCESS) {                           \
            get_logger()->error("{}:{}", __FILE__, __LINE__); \
            get_logger()->error("{}", to_string(result));     \
            std::exit(1);                                     \
        }                                                     \
    }

#endif // LYRA_PLUGIN_VULKAN_VKINCLUDE_H
