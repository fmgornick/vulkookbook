#ifndef VULKAN_H
#define VULKAN_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_metal.h>

typedef struct {
    uint32_t family_index;
    uint32_t queue_count;
    float *priorities;
} queue_info_t;

typedef struct {

    VkInstance instance;
    VkDevice logical_device;
    VkSurfaceKHR surface;
    VkPresentModeKHR present_mode;

    // extension information
    uint32_t desired_device_extensions_count;
    const char **desired_device_extensions;
    uint32_t available_instance_extensions_count;
    VkExtensionProperties *available_instance_extensions;

    // physical device information
    uint32_t device_count;
    VkPhysicalDevice *available_devices;
    VkPhysicalDevice physical_device;
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    uint32_t available_device_extensions_count;
    VkExtensionProperties *available_device_extensions;

    // logical device information
    uint32_t queue_families_count;
    VkQueueFamilyProperties *queue_families;
    queue_info_t *queue_infos;
    VkDeviceQueueCreateInfo *queue_create_infos;
    VkDeviceCreateInfo device_create_info;

    // global-level functions
    PFN_vkCreateInstance vkCreateInstance;
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;

    // instance-level functions
    PFN_vkCreateDevice vkCreateDevice;
    PFN_vkDestroyInstance vkDestroyInstance;
    PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
    PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;

    // instance-level extension functions
    // VK_EXT_debug_utils
    PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT;
    PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT;
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
    PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT;
    PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT;
    // VK_EXT_debug_report
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
    PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
    // VK_KHR_surface
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;

    // device-level functions
    PFN_vkCreateBuffer vkCreateBuffer;
    PFN_vkDestroyDevice vkDestroyDevice;
    PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
    PFN_vkGetDeviceQueue vkGetDeviceQueue;

    // device-level extension functions
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
    PFN_vkQueuePresentKHR vkQueuePresentKHR;

} vulkan_t;

void vulkan_load_global_level_functions(vulkan_t *vulkan);
void vulkan_create_instance(vulkan_t *vulkan);
void vulkan_load_instance_level_functions(vulkan_t *vulkan);
void vulkan_load_instance_level_extension_functions(vulkan_t *vulkan);
void vulkan_create_physical_device(vulkan_t *vulkan);
void vulkan_create_logical_device(vulkan_t *vulkan);
void vulkan_load_device_level_functions(vulkan_t *vulkan);
void vulkan_load_device_level_extension_functions(vulkan_t *vulkan);
void vulkan_free_resources(vulkan_t *vulkan);

#endif // VULKAN_H
