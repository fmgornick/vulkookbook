#include <stdlib.h>
#define VK_NO_PROTOTYPES
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

#if defined _WIN64
#define lib_open LoadLibrary
#define lib_addr GetProcAddress
#elif defined __linux__
#define lib_open (lib) dlopen (lib, RTLD_NOW)
#define lib_addr dlsym
#elif defined __APPLE__
#define lib_open(lib) dlopen (lib, RTLD_NOW)
#define lib_addr dlsym
#endif

void *vulkan_library;
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkCreateInstance vkCreateInstance;

int
main (int argc, char *argv[])
{
    vulkan_library = lib_open ("libvulkan.dylib");
    if (vulkan_library == NULL)
    {
        printf ("could not load vulkan library\n");
        return 1;
    }

#define EXPORTED_VULKAN_FUNCTION(name)                                  \
    name = (PFN_##name)lib_addr (vulkan_library, #name);                \
    if (name == NULL)                                                   \
    {                                                                   \
        printf ("could not load exported vulkan function %s\n", #name); \
        return 1;                                                       \
    }

#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                  \
    name = (PFN_##name)vkGetInstanceProcAddr (NULL, #name);                 \
    if (name == NULL)                                                       \
    {                                                                       \
        printf ("could not load global level vulkan function %s\n", #name); \
        return 1;                                                           \
    }

#include "vulkan_functions.inl"

    uint32_t extensions_count = 0;
    VkResult result = VK_SUCCESS;
    result = vkEnumerateInstanceExtensionProperties (NULL, &extensions_count, NULL);
    if (result != VK_SUCCESS || extensions_count == 0)
    {
        printf ("could not get instance extensions\n");
        return 1;
    }

    VkExtensionProperties *available_extensions = malloc (sizeof (VkExtensionProperties) * extensions_count);
    vkEnumerateInstanceExtensionProperties (NULL, &extensions_count, available_extensions);
    if (result != VK_SUCCESS || extensions_count == 0)
    {
        printf ("could not get instance extensions\n");
        return 1;
    }

    for (int i = 0; i < extensions_count; i++)
    {
        printf ("%s\n", available_extensions[i].extensionName);
    }

    return 0;
}
