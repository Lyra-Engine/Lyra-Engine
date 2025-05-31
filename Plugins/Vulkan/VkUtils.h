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
#include <Common/Slotmap.h>
#include <Common/Container.h>
#include <Render/RHI/API.h>

using namespace lyra;
using namespace lyra::rhi;

// define the Vulkan API version we are going to use
constexpr uint VKAPIVersion = VK_API_VERSION_1_2;

static const char* KHR_PORTABILITY_EXTENSION_NAME = "VK_KHR_portability_subset";
static const char* LUNARG_VALIDATION_LAYER_NAME   = "VK_LAYER_KHRONOS_validation";

template <typename T>
struct VulkanDestroyer
{
    void operator()(T& obj)
    {
        obj.destroy();
    }
};

template <typename T>
using VulkanResourceManager = Slotmap<T, VulkanDestroyer<T>>;

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

struct VulkanBuffer
{
    VkBuffer          buffer = VK_NULL_HANDLE;
    VmaAllocation     allocation;
    VmaAllocationInfo alloc_info;

    // implementation in VkBuffer.cpp
    void destroy();
};

struct VulkanTexture
{
    VkImage            image = VK_NULL_HANDLE;
    VmaAllocation      allocation;
    VmaAllocationInfo  alloc_info;
    VkImageAspectFlags aspects = 0;

    // implementation in VkImage.cpp
    void destroy();
};

struct VulkanSampler
{
    VkSampler sampler = VK_NULL_HANDLE;

    // implementation in VkSampler.cpp
    void destroy();
};

struct VulkanFence
{
    /**
     * NOTE that this fence implementation is based on timeline semaphores.
     * We simplified the fence concept here.
     **/
    VkSemaphore semaphore = VK_NULL_HANDLE;

    // implementation in VkFence.cpp
    void destroy();
};

struct VulkanQuery
{
    VkQueryPool pool = VK_NULL_HANDLE;

    // implementation in VkQuery.cpp
    void destroy() {}
};

struct VulkanShader
{
    VkShaderModule module = VK_NULL_HANDLE;

    // implementation in VkShader.cpp
    void destroy();
};

struct VulkanBindGroupLayout
{
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;

    // implementation in VkLayout.cpp
    void destroy();
};

struct VulkanPipelineLayout
{
    VkPipelineLayout layout = VK_NULL_HANDLE;

    // implementation in VkLayout.cpp
    void destroy();
};

struct VulkanPipeline
{
    VkPipeline      pipeline = VK_NULL_HANDLE;
    VkPipelineCache cache    = VK_NULL_HANDLE;

    // implementation in VkPipeline.cpp
    void destroy();
};

struct VulkanRHI
{
    RHIFlags           rhiflags  = 0;
    VkInstance         instance  = VK_NULL_HANDLE;
    VkSurfaceKHR       surface   = VK_NULL_HANDLE;
    VkPhysicalDevice   adapter   = VK_NULL_HANDLE;
    VkDevice           device    = VK_NULL_HANDLE;
    VkSwapchainKHR     swapchain = VK_NULL_HANDLE;
    VkExtent2D         swapchain_dim;
    VkFormat           swapchain_format;
    VkColorSpaceKHR    swapchain_colorspace;
    VolkDeviceTable    vtable;
    VmaAllocator       alloc;
    QueueFamilyIndices queues;

    // collection of objects
    VulkanResourceManager<VulkanFence>           fences;
    VulkanResourceManager<VulkanQuery>           queries;
    VulkanResourceManager<VulkanBuffer>          buffers;
    VulkanResourceManager<VulkanTexture>         textures;
    VulkanResourceManager<VulkanSampler>         samplers;
    VulkanResourceManager<VulkanShader>          shaders;
    VulkanResourceManager<VulkanPipeline>        pipelines;
    VulkanResourceManager<VulkanPipelineLayout>  pipeline_layouts;
    VulkanResourceManager<VulkanBindGroupLayout> bind_group_layouts;
};

auto get_logger() -> Logger;

// to_string
auto to_string(VkResult result) -> CString;
auto to_string(VkDebugUtilsMessageTypeFlagsEXT type) -> CString;
auto to_string(VkDebugUtilsMessageSeverityFlagBitsEXT severity) -> CString;

// enum conversions
auto vkenum(GPUPresentMode mode) -> VkPresentModeKHR;
auto vkenum(GPUCompositeAlphaMode mode) -> VkCompositeAlphaFlagBitsKHR;
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
auto vkenum(GPUIntegerCoordinate samples) -> VkSampleCountFlagBits;
auto vkenum(GPUBindingResourceType type) -> VkDescriptorType;
auto vkenum(GPUColorWriteFlags color) -> VkColorComponentFlags;
auto vkenum(GPUBufferUsageFlags usages) -> VkBufferUsageFlags;
auto vkenum(GPUTextureUsageFlags usages) -> VkImageUsageFlags;
auto vkenum(GPUShaderStageFlags stages) -> VkShaderStageFlags;
auto vkenum(GPUBarrierSyncFlags flags) -> VkPipelineStageFlags;
auto vkenum(GPUBarrierAccessFlags flags) -> VkAccessFlagBits;

bool is_binding_array(GPUBindingResourceType type);

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
bool create_buffer(GPUBufferHandle& buffer, const GPUBufferDescriptor& desc);
void delete_buffer(GPUBufferHandle buffer);
auto create_buffer(const GPUBufferDescriptor& desc) -> VulkanBuffer;
void delete_buffer(VulkanBuffer& buffer);

// vulkan texture
bool create_texture(GPUTextureHandle& texture, const GPUTextureDescriptor& desc);
void delete_texture(GPUTextureHandle texture);
auto create_texture(const GPUTextureDescriptor& desc) -> VulkanTexture;
void delete_texture(VulkanTexture& buffer);

// vulkan sampler
bool create_sampler(GPUSamplerHandle& sampler, const GPUSamplerDescriptor& desc);
void delete_sampler(GPUSamplerHandle sampler);
auto create_sampler(const GPUSamplerDescriptor& desc) -> VulkanSampler;
void delete_sampler(VulkanSampler& buffer);

// vulkan fence
bool create_fence(GPUFenceHandle& fence);
void delete_fence(GPUFenceHandle fence);
auto create_fence() -> VulkanFence;
void delete_fence(VulkanFence& buffer);

// vulkan shader
bool create_shader_module(GPUShaderModuleHandle& handle, const GPUShaderModuleDescriptor& desc);
void delete_shader_module(GPUShaderModuleHandle handle);
auto create_shader_module(const GPUShaderModuleDescriptor& desc) -> VulkanShader;
void delete_shader_module(VulkanShader& shader);

// vulkan bind group layouts
bool create_bind_group_layout(GPUBindGroupLayoutHandle& handle, const GPUBindGroupLayoutDescriptor& desc);
void delete_bind_group_layout(GPUBindGroupLayoutHandle handle);
auto create_bind_group_layout(const GPUBindGroupLayoutDescriptor& desc) -> VulkanBindGroupLayout;
void delete_bind_group_layout(VulkanBindGroupLayout& layout);

// vulkan pipeline layouts
bool create_pipeline_layout(GPUPipelineLayoutHandle& handle, const GPUPipelineLayoutDescriptor& desc);
void delete_pipeline_layout(GPUPipelineLayoutHandle handle);
auto create_pipeline_layout(const GPUPipelineLayoutDescriptor& desc) -> VulkanPipelineLayout;
void delete_pipeline_layout(VulkanPipelineLayout& layout);

// adapter/device utils
bool has_portability_subset(VkPhysicalDevice physicalDevice);
auto get_supported_instance_extensions() -> HashSet<String>;
auto get_supported_device_extensions(VkPhysicalDevice device) -> HashSet<String>;
auto find_queue_family_indices(VkPhysicalDevice device, VkSurfaceKHR surface) -> QueueFamilyIndices;

// swaphain utils
auto query_swapchain_support(VkPhysicalDevice adapter, VkSurfaceKHR surface) -> SwapchainSupportDetails;
auto choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR;
auto choose_swap_present_mode(const Vector<VkPresentModeKHR>& availablePresentModes) -> VkPresentModeKHR;
auto choose_swap_extent(const GPUSurfaceDescriptor& desc, const VkSurfaceCapabilitiesKHR& capabilities) -> VkExtent2D;

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
