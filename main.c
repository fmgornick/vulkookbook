#define VK_NO_PROTOTYPES
// #define DEBUG

#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
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
#define DBG(fmt, args...)  // nuffin
#endif

typedef struct {
    uint32_t family_index;
    uint32_t queue_count;
    float *priorities;
} queue_info_t;

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
    assert(vulkan_library != NULL);

#define EXPORTED_VULKAN_FUNCTION(name)                  \
    name = (PFN_##name)lib_addr(vulkan_library, #name); \
    assert(name != NULL);

#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                 \
    name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name); \
    assert(name != NULL);

#include "vulkan_functions.inl"

    uint32_t desired_instance_extensions_count = 3;
    const char *desired_instance_extensions[] = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };

    VkResult result;
    uint32_t available_instance_extensions_count;
    result = vkEnumerateInstanceExtensionProperties(NULL, &available_instance_extensions_count, NULL);
    assert(result == VK_SUCCESS && available_instance_extensions_count > 0);

    VkExtensionProperties *available_instance_extensions =
        (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * available_instance_extensions_count);
    result = vkEnumerateInstanceExtensionProperties(NULL, &available_instance_extensions_count, available_instance_extensions);
    assert(result == VK_SUCCESS && available_instance_extensions_count > 0);

    for (int i = 0; i < desired_instance_extensions_count; i++) {
        bool found = false;
        for (int j = 0; j < available_instance_extensions_count; j++) {
            if (strcmp(desired_instance_extensions[i], available_instance_extensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }
        assert(found);
    }

#ifdef DEBUG
    printf("INSTANCE EXTENSIONS\n");
    for (int i = 0; i < available_instance_extensions_count; i++) printf("%s\n", available_instance_extensions[i].extensionName);
    printf("\n");
#endif

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

    result = vkCreateInstance(&instance_create_info, NULL, &instance);
    assert(result == VK_SUCCESS && instance != VK_NULL_HANDLE);

#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                   \
    name = (PFN_##name)vkGetInstanceProcAddr(instance, #name); \
    assert(name != NULL);

#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                 \
    for (int i = 0; i < available_instance_extensions_count; i++) {                    \
        if (strcmp(#extension, available_instance_extensions[i].extensionName) == 0) { \
            name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                 \
            assert(name != NULL);                                                      \
        }                                                                              \
    }

#include "vulkan_functions.inl"

    uint32_t device_count;
    result = vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    assert(result == VK_SUCCESS && device_count > 0);

    VkPhysicalDevice *available_devices = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * device_count);
    result = vkEnumeratePhysicalDevices(instance, &device_count, available_devices);
    assert(result == VK_SUCCESS && device_count > 0);

    assert(device_count == 1);
    VkPhysicalDevice physical_device = available_devices[0];
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures available_device_features;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &available_device_features);

    assert(available_device_features.tessellationShader);
    available_device_features = (VkPhysicalDeviceFeatures){.tessellationShader = VK_TRUE};

    uint32_t desired_device_extensions_count = 2;
    const char *desired_device_extensions[] = {
        VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
        VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
    };

    uint32_t available_device_extensions_count;
    result = vkEnumerateDeviceExtensionProperties(physical_device, NULL, &available_device_extensions_count, NULL);
    assert(result == VK_SUCCESS && available_device_extensions_count > 0);

    VkExtensionProperties *available_device_extensions =
        (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * available_device_extensions_count);
    result = vkEnumerateDeviceExtensionProperties(physical_device, NULL, &available_device_extensions_count,
                                                  available_device_extensions);
    assert(result == VK_SUCCESS && available_device_extensions_count > 0);

    for (int i = 0; i < desired_device_extensions_count; i++) {
        bool found = false;
        for (int j = 0; j < available_device_extensions_count; j++) {
            if (strcmp(desired_device_extensions[i], available_device_extensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }
        assert(found);
    }

#ifdef DEBUG
    printf("DEVICE EXTENSIONS (%s)\n", device_properties.deviceName);
    for (int i = 0; i < available_device_extensions_count; i++) printf("%s\n", available_device_extensions[i].extensionName);
    printf("\n");
#endif

    uint32_t queue_families_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, NULL);
    assert(queue_families_count > 0);

    VkQueueFamilyProperties *queue_families =
        (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queue_families_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families);
    assert(queue_families_count > 0);

    VkQueueFlags desired_capabilities = VK_QUEUE_GRAPHICS_BIT;
    queue_info_t *queue_infos = (queue_info_t *)malloc(sizeof(queue_info_t) * queue_families_count);
    for (uint32_t i = 0; i < queue_families_count; i++) {
        VkQueueFamilyProperties queue_family = queue_families[i];
        assert(queue_family.queueFlags & desired_capabilities);
        queue_infos[i].family_index = i;
        queue_infos[i].queue_count = queue_family.queueCount;
        queue_infos[i].priorities = (float *)malloc(sizeof(float) * queue_family.queueCount);
        for (int j = 0; j < queue_family.queueCount; j++) queue_infos[i].priorities[j] = 1.0 / ((float)queue_family.queueCount);
    }

    VkDeviceQueueCreateInfo *queue_create_infos =
        (VkDeviceQueueCreateInfo *)malloc(sizeof(VkDeviceQueueCreateInfo) * queue_families_count);
    for (int i = 0; i < queue_families_count; i++) {
        queue_info_t info = queue_infos[i];
        queue_create_infos[i] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = info.family_index,
            .queueCount = info.queue_count,
            .pQueuePriorities = info.priorities,
        };
    }

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = queue_families_count,
        .pQueueCreateInfos = queue_create_infos,
        .enabledExtensionCount = desired_device_extensions_count,
        .ppEnabledExtensionNames = desired_device_extensions,
        .pEnabledFeatures = &available_device_features,
    };

    VkDevice logical_device;
    result = vkCreateDevice(physical_device, &device_create_info, NULL, &logical_device);
    assert(result == VK_SUCCESS && logical_device != VK_NULL_HANDLE);

    for (int i = 0; i < queue_families_count; i++) free(queue_infos[i].priorities);
    free(queue_infos);
    free(queue_families);
    free(available_device_extensions);
    free(available_instance_extensions);
    free(available_devices);

    return 0;
}
