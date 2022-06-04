#define VMA_IMPLEMENTATION
#ifndef NDEBUG
#ifndef VMA_DEBUG_LOG
#define VMA_DEBUG_LOG(format, ...) do { printf(format "\n" __VA_OPT__(,) __VA_ARGS__); } while(false)
#endif
#endif
#include "Context.hpp"

#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"

#include <set>
#include <vulkan/vulkan_beta.h>

using LoopEngine::Core::Singleton;
using LoopEngine::Graphics::Context;

vk::DispatchLoaderDynamic vk::defaultDispatchLoaderDynamic{};

static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        spdlog::debug("{}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        spdlog::info("{}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        spdlog::warn("{}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        spdlog::error("{}", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

static auto get_supported_format(vk::PhysicalDevice device, std::span<const vk::Format> formats, vk::FormatFeatureFlags flags) -> vk::Format {
    for (auto format : formats) {
        const auto properties = device.getFormatProperties(format);
        if ((properties.optimalTilingFeatures & flags) == flags) {
            return format;
        }
    }
    for (auto format : formats) {
        const auto properties = device.getFormatProperties(format);
        if ((properties.linearTilingFeatures & flags) == flags) {
            return format;
        }
    }
    return vk::Format::eUndefined;
}

static auto select_depth_format(vk::PhysicalDevice device) -> vk::Format {
    static constexpr auto formats = std::array{
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
        vk::Format::eD32Sfloat
    };
    return get_supported_format(device, formats, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

template<> Context* Singleton<Context>::instance = nullptr;

Context::Context() {
    spdlog::info("Initializing Vulkan");
    create_instance();
    create_debug_utils();
    select_physical_device();
    create_logical_device();
    create_memory_allocator();
    spdlog::info("Vulkan initialized");

    depth_format = select_depth_format(physical_device);

    spdlog::info("Depth format: {}", vk::to_string(depth_format));
}

Context::~Context() {
    spdlog::info("Cleaning up Vulkan");

    vmaDestroyAllocator(allocator);

    device.destroy();

#ifndef NDEBUG
    instance.destroyDebugUtilsMessengerEXT(debug_utils);
#endif
    instance.destroy();

    spdlog::info("Vulkan cleaned up");
}

void Context::create_instance() {
    VULKAN_HPP_DEFAULT_DISPATCHER.init(glfwGetInstanceProcAddress);

    vk::ApplicationInfo app_info{};
    app_info.setPApplicationName("Demo");
    app_info.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    app_info.setPEngineName("Loop");
    app_info.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    app_info.setApiVersion(VK_API_VERSION_1_3);

    uint32_t count = 0;
    auto raw_extensions = glfwGetRequiredInstanceExtensions(&count);
    auto extensions = std::vector<const char *>(raw_extensions, raw_extensions + count);


#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    // create a vector of layer names
    std::vector<const char*> layers{};
#ifndef NDEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    vk::InstanceCreateInfo instance_info{};
    instance_info.setPApplicationInfo(&app_info);
    instance_info.setPEnabledExtensionNames(extensions);
    instance_info.setPEnabledLayerNames(layers);

    instance = vk::createInstance(instance_info);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
}

void Context::create_debug_utils() {
#ifndef NDEBUG
    vk::DebugUtilsMessengerCreateInfoEXT debug_info{};
    debug_info.setMessageSeverity(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    debug_info.setMessageType(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
    );
    debug_info.setPfnUserCallback(debug_callback);

    debug_utils = instance.createDebugUtilsMessengerEXT(debug_info);
#endif
}

void Context::select_physical_device() {
    auto devices = instance.enumeratePhysicalDevices();
    if (devices.empty()) {
        throw std::runtime_error("No physical devices found");
    }
    physical_device = devices.front();

    spdlog::info("Selected device: {}", physical_device.getProperties().deviceName);

    // get queue family indices
    auto queue_families = physical_device.getQueueFamilyProperties();
    // find a graphics queue family index
    graphics_queue_family_index = std::numeric_limits<uint32_t>::max();
    for (uint32_t i = 0; i < queue_families.size(); i++) {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphics_queue_family_index = i;
            break;
        }
    }
    if (graphics_queue_family_index == std::numeric_limits<uint32_t>::max()) {
        spdlog::error("No graphics queue family index found");
        exit(1);
    }

    // find a present queue family index
    present_queue_family_index = std::numeric_limits<uint32_t>::max();
    for (uint32_t i = 0; i < queue_families.size(); i++) {
        auto present_support = glfwGetPhysicalDevicePresentationSupport(instance, physical_device, i);
        if (present_support) {
            present_queue_family_index = i;
            break;
        }
    }
    if (present_queue_family_index == std::numeric_limits<uint32_t>::max()) {
        spdlog::error("No present queue family index found");
        exit(1);
    }

    // find a compute queue family index
    compute_queue_family_index = std::numeric_limits<uint32_t>::max();
    for (uint32_t i = 0; i < queue_families.size(); i++) {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eCompute) {
            compute_queue_family_index = i;
            break;
        }
    }
    if (compute_queue_family_index == std::numeric_limits<uint32_t>::max()) {
        spdlog::error("No compute queue family index found");
        exit(1);
    }
}

void Context::create_logical_device() {
    float queue_priority = 1.0f;

    std::set<uint32_t> unique_queue_families = {
        graphics_queue_family_index,
        present_queue_family_index,
        compute_queue_family_index
    };

    // create a vector of queue create info structures
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos{};
    for (uint32_t queue_family_index : unique_queue_families) {
        vk::DeviceQueueCreateInfo queue_create_info{};
        queue_create_info.setQueueFamilyIndex(queue_family_index);
        queue_create_info.setQueueCount(1);
        queue_create_info.setPQueuePriorities(&queue_priority);
        queue_create_infos.push_back(queue_create_info);
    }

    // create a vector of extension names
    std::vector<const char*> extensions{};
    extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    extensions.push_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
#ifdef __APPLE__
    extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    // create a logical device create info structure
    vk::DeviceCreateInfo device_create_info{};
    device_create_info.setQueueCreateInfos(queue_create_infos);
    device_create_info.setPEnabledExtensionNames(extensions);

    // create the logical device
    device = physical_device.createDevice(device_create_info);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

    // get the graphics queue
    graphics_queue = device.getQueue(graphics_queue_family_index, 0);
    // get the present queue
    present_queue = device.getQueue(present_queue_family_index, 0);
    // get the compute queue
    compute_queue = device.getQueue(compute_queue_family_index, 0);
}

void Context::create_memory_allocator() {
    VmaVulkanFunctions functions{};
    functions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    functions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;
    functions.vkGetPhysicalDeviceProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties;
    functions.vkGetPhysicalDeviceMemoryProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties;
    functions.vkAllocateMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkAllocateMemory;
    functions.vkFreeMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkFreeMemory;
    functions.vkMapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkMapMemory;
    functions.vkUnmapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkUnmapMemory;
    functions.vkFlushMappedMemoryRanges = VULKAN_HPP_DEFAULT_DISPATCHER.vkFlushMappedMemoryRanges;
    functions.vkInvalidateMappedMemoryRanges = VULKAN_HPP_DEFAULT_DISPATCHER.vkInvalidateMappedMemoryRanges;
    functions.vkBindBufferMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory;
    functions.vkBindImageMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory;
    functions.vkGetBufferMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements;
    functions.vkGetImageMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements;
    functions.vkCreateBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateBuffer;
    functions.vkDestroyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyBuffer;
    functions.vkCreateImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateImage;
    functions.vkDestroyImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyImage;
    functions.vkCmdCopyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdCopyBuffer;
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
    /// Fetch "vkGetBufferMemoryRequirements2" on Vulkan >= 1.1, fetch "vkGetBufferMemoryRequirements2KHR" when using VK_KHR_dedicated_allocation extension.
    functions.vkGetBufferMemoryRequirements2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements2KHR;
    /// Fetch "vkGetImageMemoryRequirements 2" on Vulkan >= 1.1, fetch "vkGetImageMemoryRequirements2KHR" when using VK_KHR_dedicated_allocation extension.
    functions.vkGetImageMemoryRequirements2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements2KHR;
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
    /// Fetch "vkBindBufferMemory2" on Vulkan >= 1.1, fetch "vkBindBufferMemory2KHR" when using VK_KHR_bind_memory2 extension.
    functions.vkBindBufferMemory2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory2KHR;
    /// Fetch "vkBindImageMemory2" on Vulkan >= 1.1, fetch "vkBindImageMemory2KHR" when using VK_KHR_bind_memory2 extension.
    functions.vkBindImageMemory2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory2KHR;
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
    functions.vkGetPhysicalDeviceMemoryProperties2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties2KHR;
#endif
#if VMA_VULKAN_VERSION >= 1003000
    /// Fetch from "vkGetDeviceBufferMemoryRequirements" on Vulkan >= 1.3, but you can also fetch it from "vkGetDeviceBufferMemoryRequirementsKHR" if you enabled extension VK_KHR_maintenance4.
    functions.vkGetDeviceBufferMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceBufferMemoryRequirements;
    /// Fetch from "vkGetDeviceImageMemoryRequirements" on Vulkan >= 1.3, but you can also fetch it from "vkGetDeviceImageMemoryRequirementsKHR" if you enabled extension VK_KHR_maintenance4.
    functions.vkGetDeviceImageMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceImageMemoryRequirements;
#endif

    VmaAllocatorCreateInfo allocator_create_info{};
    allocator_create_info.physicalDevice = physical_device;
    allocator_create_info.device = device;
    allocator_create_info.pVulkanFunctions = &functions;
    allocator_create_info.instance = instance;
    allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_2;

    vmaCreateAllocator(&allocator_create_info, &allocator);
}