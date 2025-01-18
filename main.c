#include "sdl.h"
#include "vulkan.h"
#include <assert.h>
#include <stdlib.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

void blah(vulkan_t *vulkan, sdl_t *sdl) {
    VkResult res;
    VkPresentModeKHR desired_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    uint32_t present_modes_count;

    res = vulkan->vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan->physical_device, vulkan->surface, &present_modes_count, NULL);
    assert(res == VK_SUCCESS);
    VkPresentModeKHR *present_modes = (VkPresentModeKHR *)malloc(sizeof(VkPresentModeKHR *) * present_modes_count);
    res = vulkan->vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan->physical_device, vulkan->surface, &present_modes_count,
                                                            present_modes);
    assert(res == VK_SUCCESS);

    bool desired_present_mode_supported = false;
    for (int i = 0; i < present_modes_count; i++)
        if (desired_present_mode == present_modes[i])
            desired_present_mode_supported = true;

    if (desired_present_mode_supported)
        vulkan->present_mode = desired_present_mode;
    else
        vulkan->present_mode = VK_PRESENT_MODE_FIFO_KHR;

    free(present_modes);
}

int main(int argc, char *argv[]) {
    vulkan_t vulkan = {0};
    sdl_t sdl = {0};

    vulkan_load_global_level_functions(&vulkan);
    vulkan_create_instance(&vulkan);
    vulkan_load_instance_level_functions(&vulkan);
    vulkan_load_instance_level_extension_functions(&vulkan);
    vulkan_create_physical_device(&vulkan);

    sdl_create_window(&vulkan, &sdl);

    vulkan_create_logical_device(&vulkan);

    blah(&vulkan, &sdl);

    vulkan_load_device_level_functions(&vulkan);
    vulkan_load_device_level_extension_functions(&vulkan);

    sdl_free_resources(&vulkan, &sdl);
    vulkan_free_resources(&vulkan);

    return 0;
}
