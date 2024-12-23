#define VK_NO_PROTOTYPES

#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

// library
void *vulkan_library;

// instance
VkInstance instance;

// exported functions
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

// global level functions
PFN_vkCreateInstance vkCreateInstance;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;

// instance level functions
PFN_vkCreateDevice vkCreateDevice;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;

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
        printf("could not load global level vulkan function %s\n", #name); \
        return 1;                                                          \
    }

#include "vulkan_functions.inl"

    uint32_t extensions_count = 0;
    VkResult result = VK_SUCCESS;
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, NULL);
    if (result != VK_SUCCESS || extensions_count == 0) {
        printf("could not get instance extensions: %s\n", string_VkResult(result));
        return 1;
    }

    VkExtensionProperties *available_extensions = malloc(sizeof(VkExtensionProperties) * extensions_count);
    vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, available_extensions);
    if (result != VK_SUCCESS || extensions_count == 0) {
        printf("could not get instance extensions: %s\n", string_VkResult(result));
        return 1;
    }

    const char *desired_extensions[] = {
        "VK_KHR_portability_enumeration",
        "VK_EXT_debug_report",
        "VK_EXT_debug_utils",
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
        printf("could not load instance level vulkan function %s\n", #name); \
        return 1;                                                            \
    }

#include "vulkan_functions.inl"

    return 0;
}
