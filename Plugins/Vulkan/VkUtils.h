#ifndef LYRA_PLUGIN_VULKAN_VKINCLUDE_H
#define LYRA_PLUGIN_VULKAN_VKINCLUDE_H

#include <istream>
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
    VkBuffer          buffer     = VK_NULL_HANDLE;
    VmaAllocation     allocation = {};
    VmaAllocationInfo alloc_info = {};

    // implementation in VkBuffer.cpp
    void destroy();

    bool valid() const { return buffer != VK_NULL_HANDLE; }
};

struct VulkanTexture
{
    VkImage            image = VK_NULL_HANDLE;
    VmaAllocation      allocation;
    VmaAllocationInfo  alloc_info = {};
    VkImageAspectFlags aspects    = 0;

    // implementation in VkImage.cpp
    void destroy();

    bool valid() const { return image != VK_NULL_HANDLE; }
};

struct VulkanSampler
{
    VkSampler sampler = VK_NULL_HANDLE;

    // implementation in VkSampler.cpp
    void destroy();

    bool valid() const { return sampler != VK_NULL_HANDLE; }
};

struct VulkanFence
{
    /**
     * NOTE that this fence implementation is based on timeline semaphores.
     * We simplified the fence concept here.
     **/
    VkSemaphore semaphore = VK_NULL_HANDLE;

    // we will have to support binary semaphores in some minimal cases
    VkSemaphoreType type = VK_SEMAPHORE_TYPE_TIMELINE;

    // keep track of target value
    uint64_t target = 0ull;

    // implementation in VkFence.cpp
    void wait();
    void reset();
    bool ready();
    void destroy();

    bool valid() const { return semaphore != VK_NULL_HANDLE; }
};

struct VulkanQuery
{
    VkQueryPool pool = VK_NULL_HANDLE;

    // implementation in VkQuery.cpp
    void destroy() {}

    bool valid() const { return pool != VK_NULL_HANDLE; }
};

struct VulkanShader
{
    VkShaderModule module = VK_NULL_HANDLE;

    // implementation in VkShader.cpp
    void destroy();

    bool valid() const { return module != VK_NULL_HANDLE; }
};

struct VulkanBindGroupLayout
{
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;

    // implementation in VkLayout.cpp
    void destroy();

    bool valid() const { return layout != VK_NULL_HANDLE; }
};

struct VulkanPipelineLayout
{
    VkPipelineLayout layout = VK_NULL_HANDLE;

    // implementation in VkLayout.cpp
    void destroy();

    bool valid() const { return layout != VK_NULL_HANDLE; }
};

struct VulkanPipeline
{
    VkPipeline      pipeline = VK_NULL_HANDLE;
    VkPipelineCache cache    = VK_NULL_HANDLE;

    // implementation in VkPipeline.cpp
    void destroy();

    bool valid() const { return pipeline != VK_NULL_HANDLE; }
};

struct VulkanCommandBuffer
{
    // used to check vulkan buffer usage,
    // vulkan command buffers are short-lived, only usable within the frame.
    // frame id must match VulkanFrame's id
    uint32_t frame_id = 0u;

    VkQueue         command_queue  = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;

    // CPU/GPU synchronization
    VkFence fence = VK_NULL_HANDLE;

    // GPU/GPU synchronization
    Vector<VkSemaphoreSubmitInfo> wait_semaphores   = {};
    Vector<VkSemaphoreSubmitInfo> signal_semaphores = {};

    // implementation in VkCommandBuffer.cpp
    void wait(const VulkanFence& fence, GPUBarrierSyncFlags sync);
    void signal(const VulkanFence& fence, GPUBarrierSyncFlags sync);
    void submit();
    void begin();
    void end();
};

struct VulkanFrame
{
    // used to check vulkan buffer usage,
    // vulkan command buffers are short-lived, only usable within the frame.
    // frame id must match VulkanFrame's id
    uint32_t frame_id = 0u;

    VkCommandPool  compute_command_pool  = VK_NULL_HANDLE;
    VkCommandPool  graphics_command_pool = VK_NULL_HANDLE;
    VkCommandPool  transfer_command_pool = VK_NULL_HANDLE;
    VkFence        inflight_fence        = VK_NULL_HANDLE;
    GPUFenceHandle image_available_semaphore; // must be binary semaphores
    GPUFenceHandle render_complete_semaphore; // could be timeline semaphores

    // allocate command buffers
    Vector<VulkanCommandBuffer> allocated_command_buffers;

    // implementation in VkFrame.cpp
    void init();
    void wait();
    void reset();
    auto allocate(GPUQueueType type, bool primary) -> GPUCommandEncoderHandle;
    void destroy();
};

struct VulkanRHI
{
    RHIFlags           rhiflags       = 0;
    VkInstance         instance       = VK_NULL_HANDLE;
    VkSurfaceKHR       surface        = VK_NULL_HANDLE;
    VkPhysicalDevice   adapter        = VK_NULL_HANDLE;
    VkDevice           device         = VK_NULL_HANDLE;
    VkQueue            transfer_queue = VK_NULL_HANDLE;
    VkQueue            graphics_queue = VK_NULL_HANDLE;
    VkQueue            compute_queue  = VK_NULL_HANDLE;
    VkQueue            present_queue  = VK_NULL_HANDLE;
    VkSwapchainKHR     swapchain      = VK_NULL_HANDLE;
    VkExtent2D         swapchain_dim;
    VkFormat           swapchain_format;
    VkColorSpaceKHR    swapchain_colorspace;
    VolkDeviceTable    vtable = {};
    VmaAllocator       alloc;
    QueueFamilyIndices queues;

    // frame objects
    Vector<VulkanFrame>      frames = {};
    Vector<GPUTextureHandle> images = {};

    // frame tracker
    uint current_frame_index = 0;
    uint current_image_index = 0;

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

    auto current_frame() -> VulkanFrame& { return frames.at(current_frame_index % frames.size()); }
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
auto vkenum(GPUBarrierSyncFlags flags) -> VkPipelineStageFlags2;
auto vkenum(GPUBarrierAccessFlags flags) -> VkAccessFlagBits2;

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
auto create_fence(VkSemaphoreType type) -> VulkanFence;
void delete_fence(VulkanFence& buffer);
auto create_vkfence(bool signaled) -> VkFence;
void delete_vkfence(VkFence vkfence);
void reset_vkfence(VkFence vkfence);
void wait_vkfence(VkFence vkfence, uint64_t timeout = UINT64_MAX);

bool create_fence(GPUFenceHandle& fence, VkSemaphoreType type);
auto create_binary_semaphore() -> VulkanFence;
auto create_timeline_semaphore() -> VulkanFence;
void reset_timeline_semaphore();
void signal_timeline_semaphore(VulkanFence fence, uint64_t value);
void wait_timeline_semaphore(VulkanFence fence, uint64_t timeout = UINT64_MAX);
void reset_timeline_semaphore(VulkanFence& fence);
bool is_timeline_semaphore_ready(VulkanFence& fence);

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

// vulkan pipelines creation
bool create_render_pipeline(GPURenderPipelineHandle& handle, const GPURenderPipelineDescriptor& desc);
bool create_compute_pipeline(GPUComputePipelineHandle& handle, const GPUComputePipelineDescriptor& desc);
bool create_raytracing_pipeline(GPURayTracingPipelineHandle& handle, const GPURayTracingPipelineDescriptor& desc);

// vulkan pipelines creation
auto create_render_pipeline(const GPURenderPipelineDescriptor& desc) -> VulkanPipeline;
auto create_compute_pipeline(const GPUComputePipelineDescriptor& desc) -> VulkanPipeline;
auto create_raytracing_pipeline(const GPURayTracingPipelineDescriptor& desc) -> VulkanPipeline;

// vulkan pipelines deletion
void delete_render_pipeline(GPURenderPipelineHandle pipeline);
void delete_compute_pipeline(GPUComputePipelineHandle pipeline);
void delete_raytracing_pipeline(GPURayTracingPipelineHandle pipeline);
void delete_pipeline(VulkanPipeline& pipeline);

// vulkan command pool
auto create_command_pool(uint queue_family_index) -> VkCommandPool;
void delete_command_pool(VkCommandPool pool);
void reset_command_pool(VkCommandPool pool);

// vulkan command buffer
auto allocate_command_buffer(VkCommandPool pool, bool primary = true) -> VkCommandBuffer;
bool create_command_buffer(GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBufferDescriptor& descriptor);
bool create_command_bundle(GPUCommandEncoderHandle& cmdbuffer, const GPUCommandBufferDescriptor& descriptor);
void begin_command_buffer(VkCommandBuffer cmdbuffer);
void end_command_buffer(VkCommandBuffer cmdbuffer);

// vulkan swapchain utils
bool acquire_next_frame(GPUTextureHandle& texture, GPUFenceHandle& image_available_fence, GPUFenceHandle& render_complete_fence, bool& suboptimal);
bool present_curr_frame(GPUTextureHandle texture);

// device/queue related
void wait_idle();
void wait_fence(GPUFenceHandle handle);

// vulkan command buffer recording
namespace cmd
{
    void set_render_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURenderPipelineHandle pipeline);
    void set_compute_pipeline(GPUCommandEncoderHandle cmdbuffer, GPUComputePipelineHandle pipeline);
    void set_raytracing_pipeline(GPUCommandEncoderHandle cmdbuffer, GPURayTracingPipelineHandle pipeline);
    void set_bind_group(GPUCommandEncoderHandle cmdbuffer, GPUPipelineLayoutHandle layout, GPUIndex32 index, GPUBindGroupHandle bind_group, const Vector<GPUBufferDynamicOffset>& dynamic_offsets);
    void set_index_buffer(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle buffer, GPUIndexFormat format, GPUSize64 offset, GPUSize64 size);
    void set_vertex_buffer(GPUCommandEncoderHandle cmdbuffer, GPUIndex32 slot, GPUBufferHandle buffer, GPUSize64 offset, GPUSize64 size);
    void draw(GPUCommandEncoderHandle cmdbuffer, GPUSize32 vertex_count, GPUSize32 instance_count, GPUSize32 first_vertex, GPUSize32 first_instance);
    void draw_indexed(GPUCommandEncoderHandle cmdbuffer, GPUSize32 index_count, GPUSize32 instance_count, GPUSize32 first_index, GPUSignedOffset32 base_vertex, GPUSize32 first_instance);
    void draw_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset);
    void draw_indexed_indirect(GPUCommandEncoderHandle cmdbuffer, GPUBufferHandle indirect_buffer, GPUSize64 indirect_offset);
} // namespace cmd

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

template <typename T, typename Handle>
T& fetch_resource(VulkanResourceManager<T>& manager, Handle handle)
{
    // check handle validity
    if (!handle.valid()) {
        get_logger()->error("Resource handle {} is invalid!", typeid(Handle).name());
        exit(1);
    }

    // check resource range
    if (handle.value >= manager.data.size()) {
        get_logger()->error("Resource handle {} with value={} access out of range!", typeid(Handle).name(), handle.value);
        exit(1);
    }

    T& resource = manager.data.at(handle.value);
    if (!resource.valid()) {
        get_logger()->error("Resource handle {} with value={} has invalid Vk object!", typeid(Handle).name(), handle.value);
        exit(1);
    }
    return resource;
}

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
