#include "VkUtils.h"

#define VK_LOAD(HANDLE, FUNC) \
    (HANDLE)->vtable.FUNC = (PFN_##FUNC)vkGetDeviceProcAddr((HANDLE)->device, #FUNC);

bool is_supported(const Vector<CString>& extensions, CString name)
{
    size_t length = strnlen(name, 20);
    for (auto& extension : extensions)
        if (strncmp(extension, name, length) == 0)
            return true;
    return false;
}

bool is_required(const Vector<GPUFeatureName>& features, GPUFeatureName feature)
{
    return std::find(features.begin(), features.end(), feature) != features.end();
}

void create_allocator(VulkanRHI* rhi, bool enable_buffer_device_address)
{
    // need to manually map functions to vma
    VmaVulkanFunctions vulkan_functions;
    vulkan_functions.vkGetPhysicalDeviceProperties           = vkGetPhysicalDeviceProperties;
    vulkan_functions.vkGetPhysicalDeviceMemoryProperties     = vkGetPhysicalDeviceMemoryProperties;
    vulkan_functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
    vulkan_functions.vkAllocateMemory                        = rhi->vtable.vkAllocateMemory;
    vulkan_functions.vkFreeMemory                            = rhi->vtable.vkFreeMemory;
    vulkan_functions.vkMapMemory                             = rhi->vtable.vkMapMemory;
    vulkan_functions.vkUnmapMemory                           = rhi->vtable.vkUnmapMemory;
    vulkan_functions.vkFlushMappedMemoryRanges               = rhi->vtable.vkFlushMappedMemoryRanges;
    vulkan_functions.vkInvalidateMappedMemoryRanges          = rhi->vtable.vkInvalidateMappedMemoryRanges;
    vulkan_functions.vkBindBufferMemory                      = rhi->vtable.vkBindBufferMemory;
    vulkan_functions.vkBindImageMemory                       = rhi->vtable.vkBindImageMemory;
    vulkan_functions.vkGetBufferMemoryRequirements           = rhi->vtable.vkGetBufferMemoryRequirements;
    vulkan_functions.vkGetImageMemoryRequirements            = rhi->vtable.vkGetImageMemoryRequirements;
    vulkan_functions.vkCreateBuffer                          = rhi->vtable.vkCreateBuffer;
    vulkan_functions.vkDestroyBuffer                         = rhi->vtable.vkDestroyBuffer;
    vulkan_functions.vkCreateImage                           = rhi->vtable.vkCreateImage;
    vulkan_functions.vkDestroyImage                          = rhi->vtable.vkDestroyImage;
    vulkan_functions.vkCmdCopyBuffer                         = rhi->vtable.vkCmdCopyBuffer;
    vulkan_functions.vkGetBufferMemoryRequirements2KHR       = rhi->vtable.vkGetBufferMemoryRequirements2KHR;
    vulkan_functions.vkGetImageMemoryRequirements2KHR        = rhi->vtable.vkGetImageMemoryRequirements2KHR;
    vulkan_functions.vkBindBufferMemory2KHR                  = rhi->vtable.vkBindBufferMemory2KHR;
    vulkan_functions.vkBindImageMemory2KHR                   = rhi->vtable.vkBindImageMemory2KHR;

    // create allocator
    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.instance               = rhi->instance;
    allocator_info.physicalDevice         = rhi->adapter;
    allocator_info.device                 = rhi->device;
    allocator_info.flags                  = 0;
    allocator_info.pVulkanFunctions       = (const VmaVulkanFunctions*)&vulkan_functions;

    // enable buffer device address capability for vma allocator
    if (enable_buffer_device_address)
        allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    vk_check(vmaCreateAllocator(&allocator_info, &rhi->alloc));
}

bool create_device(const GPUDeviceDescriptor& desc)
{
    auto logger = get_logger();

    auto rhi = get_rhi();

    Vector<CString> device_extensions   = {};
    Vector<CString> debugger_extensions = {};
    Vector<CString> validation_layers   = {};

    auto required_features       = GPUSupportedFeatures{};
    required_features.bindless   = is_required(desc.required_features, GPUFeatureName::BINDLESS);
    required_features.raytracing = is_required(desc.required_features, GPUFeatureName::RAYTRACING);

    // add validation layer
    if (rhi->rhiflags.contains(RHIFlag::VALIDATION)) {
        validation_layers.push_back(LUNARG_VALIDATION_LAYER_NAME);
    }

    // add debug utils extension
    if (rhi->rhiflags.contains(RHIFlag::DEBUG)) {
        debugger_extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
    }

    // add portability (if present)
    if (has_portability_subset(rhi->adapter)) {
        device_extensions.push_back(KHR_PORTABILITY_EXTENSION_NAME);
    }

    // always loaded extensions
    device_extensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
    device_extensions.push_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);

    // load swapchain extensions
    if (rhi->surface) {
        device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    // load bindless extensions
    if (required_features.bindless)
        device_extensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

    // load raytracing extensions
    if (required_features.raytracing) {
        device_extensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
    }

    // features
    VkPhysicalDeviceFeatures2 features  = {};
    features.sType                      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext                      = nullptr;
    features.features.shaderInt64       = VK_TRUE;
    features.features.multiDrawIndirect = VK_TRUE;
    features.features.fillModeNonSolid  = VK_TRUE;

    // append to features linked list
    auto append_feature = [&](VulkanBase* feature) {
        feature->pNext = features.pNext;
        features.pNext = feature;
    };

    // timeline semaphores (essential: used to support unified GPU/CPU fence)
    auto timeline_semaphore = VkPhysicalDeviceTimelineSemaphoreFeatures{};
    {
        device_extensions.push_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
        timeline_semaphore.sType             = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;
        timeline_semaphore.pNext             = nullptr;
        timeline_semaphore.timelineSemaphore = VK_TRUE;
        append_feature((VulkanBase*)&timeline_semaphore);
        if (!is_supported(device_extensions, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
            exit(1);
        }
    }

    // imageless framebuffer (essential: used to support detached framebuffer)
    auto imageless_framebuffer = VkPhysicalDeviceImagelessFramebufferFeatures{};
    {
        device_extensions.push_back(VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME);
        imageless_framebuffer.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES;
        imageless_framebuffer.pNext                = nullptr;
        imageless_framebuffer.imagelessFramebuffer = VK_TRUE;
        append_feature((VulkanBase*)&imageless_framebuffer);
        if (!is_supported(device_extensions, VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME);
            exit(1);
        }
    }

    // dynamic rendering (essential: used to support rendering without render pass)
    auto dynamic_rendering = VkPhysicalDeviceDynamicRenderingFeatures{};
    {
        device_extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        dynamic_rendering.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamic_rendering.pNext            = nullptr;
        dynamic_rendering.dynamicRendering = VK_TRUE;
        append_feature((VulkanBase*)&dynamic_rendering);
        if (!is_supported(device_extensions, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
            exit(1);
        }
    }

    // optional: used to support bindless descriptors
    auto descriptor_indexing = VkPhysicalDeviceDescriptorIndexingFeatures{};
    if (required_features.bindless) {
        descriptor_indexing.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descriptor_indexing.pNext                                     = nullptr;
        descriptor_indexing.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        descriptor_indexing.runtimeDescriptorArray                    = VK_TRUE;
        descriptor_indexing.descriptorBindingVariableDescriptorCount  = VK_TRUE;
        descriptor_indexing.descriptorBindingPartiallyBound           = VK_TRUE;
        append_feature((VulkanBase*)&descriptor_indexing);
        if (!is_supported(device_extensions, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
            exit(1);
        }
    }

    // optional: used to get device address for raytracing buffers
    auto buffer_device_address = VkPhysicalDeviceBufferDeviceAddressFeatures{};
    if (required_features.raytracing) {
        buffer_device_address.sType               = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        buffer_device_address.pNext               = nullptr;
        buffer_device_address.bufferDeviceAddress = VK_TRUE;
        append_feature((VulkanBase*)&buffer_device_address);
        if (!is_supported(device_extensions, VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
            exit(1);
        }
    }

    // optional: used for host query reset in raytracing
    auto host_query_reset = VkPhysicalDeviceHostQueryResetFeatures{};
    if (required_features.raytracing) {
        host_query_reset.sType          = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
        host_query_reset.pNext          = nullptr;
        host_query_reset.hostQueryReset = VK_TRUE;
        append_feature((VulkanBase*)&host_query_reset);
        if (!is_supported(device_extensions, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME);
            exit(1);
        }
    }

    // optional: used to support bvh building
    auto acceleration_structure = VkPhysicalDeviceAccelerationStructureFeaturesKHR{};
    if (required_features.raytracing) {
        acceleration_structure.sType                 = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        acceleration_structure.pNext                 = nullptr;
        acceleration_structure.accelerationStructure = VK_TRUE;
        append_feature((VulkanBase*)&acceleration_structure);
        if (!is_supported(device_extensions, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
            exit(1);
        }
    }

    // optional: used to raytracing pipelines
    auto raytracing_pipeline = VkPhysicalDeviceRayTracingPipelineFeaturesKHR{};
    if (required_features.raytracing) {
        raytracing_pipeline.sType                        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        raytracing_pipeline.pNext                        = nullptr;
        raytracing_pipeline.rayTracingPipeline           = VK_TRUE;
        raytracing_pipeline.rayTraversalPrimitiveCulling = VK_TRUE;
        append_feature((VulkanBase*)&raytracing_pipeline);
        if (!is_supported(device_extensions, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
            exit(1);
        }
    }

    // optional: used to ray query in other shader types.
    auto ray_query = VkPhysicalDeviceRayQueryFeaturesKHR{};
    if (required_features.raytracing) {
        ray_query.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
        ray_query.pNext    = nullptr;
        ray_query.rayQuery = VK_TRUE;
        append_feature((VulkanBase*)&ray_query);
        if (!is_supported(device_extensions, VK_KHR_RAY_QUERY_EXTENSION_NAME)) {
            get_logger()->error("Device extension {} is not supported!", VK_KHR_RAY_QUERY_EXTENSION_NAME);
            exit(1);
        }
    }

    // logger
    if (!device_extensions.empty()) {
        logger->info("Create VkDevice with extensions:");
        for (const auto& extension : device_extensions) {
            logger->info("- {}", extension);
        }
    }
    if (!debugger_extensions.empty()) {
        logger->info("Create VkDevice with debug extensions:");
        for (const auto& extension : debugger_extensions) {
            logger->info("- {}", extension);
        }
    }
    if (!validation_layers.empty()) {
        logger->info("Create VkDevice with validation layers:");
        for (const auto& layer : validation_layers) {
            logger->info("- {}", layer);
        }
    }

    // check availability of extensions
    auto supported_extensions        = get_supported_device_extensions(rhi->adapter);
    uint unsatisfied_extension_count = 0;
    for (auto& extension : device_extensions) {
        auto ext = String(extension);
        if (supported_extensions.find(ext) == supported_extensions.end()) {
            logger->error("VkDevice Extension <" + ext + "> is not supported!");
            unsatisfied_extension_count++;
        }
    }
    if (unsatisfied_extension_count) {
        show_error("Vulkan Render Backend", "Missing required device extensions!");
    }

    // update queue family indices
    auto queue_family_indices = find_queue_family_indices(rhi->adapter, rhi->surface);

    // uniquify queue family indices
    HashSet<uint> unique_queue_families;
    if (queue_family_indices.compute.has_value()) unique_queue_families.insert(queue_family_indices.compute.value());
    if (queue_family_indices.graphics.has_value()) unique_queue_families.insert(queue_family_indices.graphics.value());
    if (queue_family_indices.present.has_value()) unique_queue_families.insert(queue_family_indices.present.value());

    // prepare queue infos
    float queue_priority     = 1.0f;
    auto  queue_create_infos = std::vector<VkDeviceQueueCreateInfo>{};
    for (uint index : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex        = index;
        queue_create_info.queueCount              = 1;
        queue_create_info.pQueuePriorities        = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    // prepare device info
    VkDeviceCreateInfo create_info      = {};
    create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos       = queue_create_infos.data();
    create_info.queueCreateInfoCount    = queue_create_infos.size();
    create_info.pEnabledFeatures        = nullptr;
    create_info.enabledExtensionCount   = static_cast<uint>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();
    create_info.pNext                   = &features;
    create_info.enabledLayerCount       = 0;

    // enable validation if needed
    if (rhi->rhiflags.contains(RHIFlag::VALIDATION)) {
        create_info.enabledLayerCount   = static_cast<uint>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    }

    // create logical device
    vk_check(vkCreateDevice(rhi->adapter, &create_info, nullptr, &rhi->device));
    rhi->queues = queue_family_indices;

    // load necessary functions
    volkLoadDeviceTable(&rhi->vtable, rhi->device);

    // load swapchain functions
    if (is_supported(device_extensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        VK_LOAD(rhi, vkCreateSwapchainKHR);
        VK_LOAD(rhi, vkDestroySwapchainKHR);
        VK_LOAD(rhi, vkGetSwapchainImagesKHR);
    }

    // create memory allocator
    bool enable_buffer_device_address = required_features.raytracing;
    create_allocator(rhi, enable_buffer_device_address);

    return true;
}

void delete_device()
{
    auto rhi = get_rhi();

    // clean up remaining fences
    for (auto& fence : rhi->fences.data)
        delete_fence(fence);

    // clean up remaining buffers
    for (auto& buffer : rhi->buffers.data)
        delete_buffer(buffer);

    // clean up remaining textures
    for (auto& texture : rhi->textures.data)
        delete_texture(texture);

    // clean up remaining samplers
    for (auto& sampler : rhi->samplers.data)
        delete_sampler(sampler);

    // clean up remaining shaders
    for (auto& shader : rhi->shaders.data)
        delete_shader_module(shader);

    // clean up remaining bind group layouts
    for (auto& layout : rhi->bind_group_layouts.data)
        delete_bind_group_layout(layout);

    // clean up remaining pipeline layouts
    for (auto& layout : rhi->pipeline_layouts.data)
        delete_pipeline_layout(layout);

    // clean up remaining pipelines
    for (auto& pipeline : rhi->pipelines.data)
        delete_pipeline(pipeline);

    // // clean up remaining query sets
    // for (auto& query : rhi->queries.data)
    //     delete_query_set(query);

    if (rhi->swapchain) {
        rhi->vtable.vkDestroySwapchainKHR(rhi->device, rhi->swapchain, nullptr);
        rhi->swapchain = VK_NULL_HANDLE;
    }

    if (rhi->alloc) {
        vmaDestroyAllocator(rhi->alloc);
        rhi->alloc = VK_NULL_HANDLE;
    }

    if (rhi->device) {
        rhi->vtable.vkDestroyDevice(rhi->device, nullptr);
        rhi->device = VK_NULL_HANDLE;
    }
}
