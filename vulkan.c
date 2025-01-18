#include "vulkan.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void vulkan_load_global_level_functions(vulkan_t *vulkan) {
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                         \
    vulkan->name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name); \
    assert(name);

    GLOBAL_LEVEL_VULKAN_FUNCTION(vkCreateInstance)
    GLOBAL_LEVEL_VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties)
    GLOBAL_LEVEL_VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties)

#undef GLOBAL_LEVEL_VULKAN_FUNCTION
}

void vulkan_create_instance(vulkan_t *vulkan) {
    uint32_t desired_instance_extensions_count = 5;
    const char *desired_instance_extensions[] = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_EXT_METAL_SURFACE_EXTENSION_NAME, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

    VkResult result;
    result = vulkan->vkEnumerateInstanceExtensionProperties(NULL, &vulkan->available_instance_extensions_count, NULL);
    assert(result == VK_SUCCESS && vulkan->available_instance_extensions_count > 0);

    vulkan->available_instance_extensions =
        (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * vulkan->available_instance_extensions_count);
    result = vulkan->vkEnumerateInstanceExtensionProperties(NULL, &vulkan->available_instance_extensions_count,
                                                            vulkan->available_instance_extensions);
    assert(result == VK_SUCCESS && vulkan->available_instance_extensions_count > 0);

    for (int i = 0; i < desired_instance_extensions_count; i++) {
        bool found = false;
        for (int j = 0; j < vulkan->available_instance_extensions_count; j++) {
            if (strcmp(desired_instance_extensions[i], vulkan->available_instance_extensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }
        assert(found);
    }

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "vulkookbook",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "vulkookbook engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
        .pApplicationInfo = &application_info,
        .enabledExtensionCount = desired_instance_extensions_count,
        .ppEnabledExtensionNames = desired_instance_extensions,
    };

    result = vulkan->vkCreateInstance(&instance_create_info, NULL, &vulkan->instance);
    assert(result == VK_SUCCESS && vulkan->instance != VK_NULL_HANDLE);
}

void vulkan_load_instance_level_functions(vulkan_t *vulkan) {
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                   \
    vulkan->name = (PFN_##name)vkGetInstanceProcAddr(vulkan->instance, #name); \
    assert(vulkan->name);

    INSTANCE_LEVEL_VULKAN_FUNCTION(vkCreateDevice)
    INSTANCE_LEVEL_VULKAN_FUNCTION(vkDestroyInstance)
    INSTANCE_LEVEL_VULKAN_FUNCTION(vkEnumerateDeviceExtensionProperties)
    INSTANCE_LEVEL_VULKAN_FUNCTION(vkEnumeratePhysicalDevices)
    INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetDeviceProcAddr)
    INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceFeatures)
    INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceProperties)
    INSTANCE_LEVEL_VULKAN_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)

#undef INSTANCE_LEVEL_VULKAN_FUNCTION
}

void vulkan_load_instance_level_extension_functions(vulkan_t *vulkan) {
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                        \
    for (int i = 0; i < vulkan->available_instance_extensions_count; i++) {                   \
        if (strcmp(extension, vulkan->available_instance_extensions[i].extensionName) == 0) { \
            vulkan->name = (PFN_##name)vkGetInstanceProcAddr(vulkan->instance, #name);        \
            assert(vulkan->name);                                                             \
        }                                                                                     \
    }

    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCmdBeginDebugUtilsLabelEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCmdEndDebugUtilsLabelEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCmdInsertDebugUtilsLabelEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateDebugUtilsMessengerEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroyDebugUtilsMessengerEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkQueueBeginDebugUtilsLabelEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkQueueEndDebugUtilsLabelEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkQueueInsertDebugUtilsLabelEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkSetDebugUtilsObjectNameEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkSetDebugUtilsObjectTagEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkSubmitDebugUtilsMessageEXT, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)

    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateDebugReportCallbackEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDebugReportMessageEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroyDebugReportCallbackEXT, VK_EXT_DEBUG_REPORT_EXTENSION_NAME)

    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfacePresentModesKHR, VK_KHR_SURFACE_EXTENSION_NAME)
    INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceSupportKHR, VK_KHR_SURFACE_EXTENSION_NAME)

#undef INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
}

void vulkan_create_physical_device(vulkan_t *vulkan) {
    VkResult result = vkEnumeratePhysicalDevices(vulkan->instance, &vulkan->device_count, NULL);
    assert(result == VK_SUCCESS && vulkan->device_count > 0);

    vulkan->available_devices = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * vulkan->device_count);
    result = vkEnumeratePhysicalDevices(vulkan->instance, &vulkan->device_count, vulkan->available_devices);
    assert(result == VK_SUCCESS && vulkan->device_count > 0);

    assert(vulkan->device_count == 1);
    vulkan->physical_device = vulkan->available_devices[0];
    vkGetPhysicalDeviceProperties(vulkan->physical_device, &vulkan->device_properties);
    vkGetPhysicalDeviceFeatures(vulkan->physical_device, &vulkan->device_features);

    assert(vulkan->device_features.tessellationShader);
    vulkan->device_features = (VkPhysicalDeviceFeatures){.tessellationShader = VK_TRUE};

    vulkan->desired_device_extensions_count = 3;
    vulkan->desired_device_extensions = (const char **)malloc(3 * sizeof(char *));
    vulkan->desired_device_extensions[0] = VK_EXT_DEBUG_MARKER_EXTENSION_NAME;
    vulkan->desired_device_extensions[1] = VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME;
    vulkan->desired_device_extensions[2] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    result = vkEnumerateDeviceExtensionProperties(vulkan->physical_device, NULL, &vulkan->available_device_extensions_count, NULL);
    assert(result == VK_SUCCESS && vulkan->available_device_extensions_count > 0);

    vulkan->available_device_extensions =
        (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * vulkan->available_device_extensions_count);
    result = vkEnumerateDeviceExtensionProperties(vulkan->physical_device, NULL, &vulkan->available_device_extensions_count,
                                                  vulkan->available_device_extensions);
    assert(result == VK_SUCCESS && vulkan->available_device_extensions_count > 0);

    for (int i = 0; i < vulkan->desired_device_extensions_count; i++) {
        bool found = false;
        for (int j = 0; j < vulkan->available_device_extensions_count; j++) {
            if (strcmp(vulkan->desired_device_extensions[i], vulkan->available_device_extensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }
        assert(found);
    }
}

void vulkan_create_logical_device(vulkan_t *vulkan) {
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physical_device, &vulkan->queue_families_count, NULL);
    assert(vulkan->queue_families_count > 0);

    vulkan->queue_families = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * vulkan->queue_families_count);
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan->physical_device, &vulkan->queue_families_count, vulkan->queue_families);
    assert(vulkan->queue_families_count > 0);

    VkQueueFlags desired_capabilities = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
    vulkan->queue_infos = (queue_info_t *)malloc(sizeof(queue_info_t) * vulkan->queue_families_count);
    for (uint32_t i = 0; i < vulkan->queue_families_count; i++) {
        VkQueueFamilyProperties queue_family = vulkan->queue_families[i];
        assert((queue_family.queueFlags & desired_capabilities) == desired_capabilities);

        VkBool32 presentation_supported = VK_FALSE;
        VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(vulkan->physical_device, i, vulkan->surface, &presentation_supported);
        assert(res == VK_SUCCESS && presentation_supported);

        vulkan->queue_infos[i].family_index = i;
        vulkan->queue_infos[i].queue_count = queue_family.queueCount;
        vulkan->queue_infos[i].priorities = (float *)malloc(sizeof(float) * queue_family.queueCount);
        for (int j = 0; j < queue_family.queueCount; j++)
            vulkan->queue_infos[i].priorities[j] = 1.0 / ((float)queue_family.queueCount);
    }

    vulkan->queue_create_infos = (VkDeviceQueueCreateInfo *)malloc(sizeof(VkDeviceQueueCreateInfo) * vulkan->queue_families_count);
    for (int i = 0; i < vulkan->queue_families_count; i++) {
        queue_info_t info = vulkan->queue_infos[i];
        vulkan->queue_create_infos[i] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = info.family_index,
            .queueCount = info.queue_count,
            .pQueuePriorities = info.priorities,
        };
    }

    vulkan->device_create_info = (VkDeviceCreateInfo){
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = vulkan->queue_families_count,
        .pQueueCreateInfos = vulkan->queue_create_infos,
        .enabledExtensionCount = vulkan->desired_device_extensions_count,
        .ppEnabledExtensionNames = vulkan->desired_device_extensions,
        .pEnabledFeatures = &vulkan->device_features,
    };

    VkResult result = vkCreateDevice(vulkan->physical_device, &vulkan->device_create_info, NULL, &vulkan->logical_device);
    assert(result == VK_SUCCESS && vulkan->logical_device != VK_NULL_HANDLE);
}

void vulkan_load_device_level_functions(vulkan_t *vulkan) {
#define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                                 \
    vulkan->name = (PFN_##name)vulkan->vkGetDeviceProcAddr(vulkan->logical_device, #name); \
    assert(vulkan->name);

    DEVICE_LEVEL_VULKAN_FUNCTION(vkCreateBuffer)
    DEVICE_LEVEL_VULKAN_FUNCTION(vkDestroyDevice)
    DEVICE_LEVEL_VULKAN_FUNCTION(vkDeviceWaitIdle)
    DEVICE_LEVEL_VULKAN_FUNCTION(vkGetBufferMemoryRequirements)
    DEVICE_LEVEL_VULKAN_FUNCTION(vkGetDeviceQueue)

#undef DEVICE_LEVEL_VULKAN_FUNCTION
}

void vulkan_load_device_level_extension_functions(vulkan_t *vulkan) {
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                               \
    for (int i = 0; i < vulkan->available_device_extensions_count; i++) {                          \
        if (strcmp(#extension, vulkan->available_device_extensions[i].extensionName) == 0) {       \
            vulkan->name = (PFN_##name)vulkan->vkGetDeviceProcAddr(vulkan->logical_device, #name); \
            assert(vulkan->name);                                                                  \
        }                                                                                          \
    }

    DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkAcquireNextImageKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkCreateSwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkDestroySwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkGetSwapchainImagesKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(vkQueuePresentKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)

#undef DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION
}

void vulkan_free_resources(vulkan_t *vulkan) {
    vkDestroyDevice(vulkan->logical_device, NULL);
    vkDestroyInstance(vulkan->instance, NULL);
    vulkan->logical_device = VK_NULL_HANDLE;
    vulkan->instance = VK_NULL_HANDLE;

    for (int i = 0; i < vulkan->queue_families_count; i++)
        free(vulkan->queue_infos[i].priorities);
    free(vulkan->queue_infos);
    free(vulkan->queue_create_infos);
    free(vulkan->queue_families);
    free(vulkan->available_device_extensions);
    free(vulkan->available_instance_extensions);
    free(vulkan->desired_device_extensions);
    free(vulkan->available_devices);
}
