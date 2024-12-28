#define VK_NO_PROTOTYPES
// #define DEBUG

#include <assert.h>
#include <dlfcn.h>
#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#if defined _WIN64
#define lib_open LoadLibrary
#define lib_addr GetProcAddress
#elif defined __linux__
#define lib_open (lib) dlopen(lib, RTLD_NOW)
#define lib_addr dlsym
#elif defined __APPLE__
#define lib_open(lib) dlopen(lib, RTLD_NOW)
#define lib_addr dlsym
#endif

#ifdef DEBUG
#define DBG(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define DBG(fmt, args...) /* Don't do anything in release builds */
#endif

// library
void *vulkan_library;

// instance
VkInstance instance;

// exported functions
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

// global-level functions
PFN_vkCreateInstance vkCreateInstance;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;

// instance-level functions
PFN_vkCreateDevice vkCreateDevice;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
PFN_vkGetDeviceQueue vkGetDeviceQueue;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;

// instance-level extensions
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

int main(int argc, char *argv[]) {
    vulkan_library = lib_open("libvulkan.dylib");
    if (vulkan_library == NULL) {
        printf("could not load vulkan library\n");
        return 1;
    }

#define EXPORTED_VULKAN_FUNCTION(name)                                 \
    name = (PFN_##name)lib_addr(vulkan_library, #name);                \
    if (name == NULL) {                                                \
        printf("could not load exported vulkan function %s\n", #name); \
        return 1;                                                      \
    }

#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                 \
    name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name);                 \
    if (name == NULL) {                                                    \
        printf("could not load global-level vulkan function %s\n", #name); \
        return 1;                                                          \
    }

#include "vulkan_functions.inl"

    uint32_t extensions_count;
    VkResult result;
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, NULL);
    if (result != VK_SUCCESS || extensions_count == 0) {
        printf("could not get instance extensions: %s\n", string_VkResult(result));
        return 1;
    }

    VkExtensionProperties *available_extensions = (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * extensions_count);
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, available_extensions);
    if (result != VK_SUCCESS || extensions_count == 0) {
        printf("could not get instance extensions: %s\n", string_VkResult(result));
        return 1;
    }

#ifdef DEBUG
    printf("INSTANCE EXTENSIONS\n");
    for (int i = 0; i < extensions_count; i++) printf("%s\n", available_extensions[i].extensionName);
    printf("\n");
#endif

    const char *desired_extensions[] = {
        "VK_EXT_debug_report",
        "VK_EXT_debug_utils",
        "VK_KHR_portability_enumeration",
    };

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "vulkookbook",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "vulkookbook engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 3,
        .ppEnabledExtensionNames = desired_extensions,
    };

    result = vkCreateInstance(&instance_create_info, NULL, &instance);
    if (result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
        printf("could not create vulkan instance: %s\n", string_VkResult(result));
        return 1;
    }

#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                 \
    name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);               \
    if (name == NULL) {                                                      \
        printf("could not load instance-level vulkan function %s\n", #name); \
        return 1;                                                            \
    }

#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                \
    for (int i = 0; i < extensions_count; i++) {                                      \
        if (strcmp(#extension, available_extensions[i].extensionName) == 0) {         \
            name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                \
            if (name == NULL) {                                                       \
                printf("could not load instance-level vulkan extension %s\n", #name); \
                return 1;                                                             \
            }                                                                         \
        }                                                                             \
    }

#include "vulkan_functions.inl"

    uint32_t device_count;
    result = vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    if (result != VK_SUCCESS || device_count == 0) {
        printf("could not enumerate physical devices: %s\n", string_VkResult(result));
        return 1;
    }

    VkPhysicalDevice *available_devices = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * device_count);
    result = vkEnumeratePhysicalDevices(instance, &device_count, available_devices);
    if (result != VK_SUCCESS || device_count == 0) {
        printf("could not enumerate physical devices: %s\n", string_VkResult(result));
        return 1;
    }

    assert(device_count == 1);
    VkPhysicalDevice device = available_devices[0];
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);

    uint32_t extension_count;
    result = vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    if (result != VK_SUCCESS || extension_count == 0) {
        printf("could not enumerate device extensions: %s\n", string_VkResult(result));
        return 1;
    }

    VkExtensionProperties *extension_properties = (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * extension_count);
    result = vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, extension_properties);
    if (result != VK_SUCCESS || extension_count == 0) {
        printf("could not enumerate device extensions: %s\n", string_VkResult(result));
        return 1;
    }

#ifdef DEBUG
    printf("DEVICE EXTENSIONS (%s)\n", device_properties.deviceName);
    for (int i = 0; i < extension_count; i++) printf("%s\n", extension_properties[i].extensionName);
    printf("\n");
#endif

    uint32_t queue_families_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, NULL);
    if (queue_families_count == 0) {
        printf("could not get device queue properties\n");
        return 1;
    }

    VkQueueFamilyProperties *queue_familes =
        (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queue_families_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, queue_familes);
    if (queue_families_count == 0) {
        printf("could not get device queue properties\n");
        return 1;
    }

    uint32_t queue_family_index;
    VkQueueFlags desired_capabilities = VK_QUEUE_GRAPHICS_BIT;
    for (uint32_t i = 0; i < queue_families_count; i++) {
        VkQueueFamilyProperties queue_family = queue_familes[i];
        if (queue_family.queueCount > 0 && queue_family.queueFlags & desired_capabilities) queue_family_index = i;
    }

    // VkQueue queue;
    // vkGetDeviceQueue(device, queue_family_index, 0, &queue);

    free(queue_familes);
    free(extension_properties);
    free(available_extensions);
    free(available_devices);

    return 0;
}
