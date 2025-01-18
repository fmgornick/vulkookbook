#include "sdl.h"
#include <assert.h>
#include <stdbool.h>

void sdl_create_window(vulkan_t *vulkan, sdl_t *sdl) {
    bool success = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    assert(success);
    success = SDL_Vulkan_LoadLibrary(NULL);
    assert(success);

    sdl->window = SDL_CreateWindow("sdl vulkan example", 512, 512, SDL_WINDOW_VULKAN);
    assert(sdl->window);
    success = SDL_Vulkan_CreateSurface(sdl->window, vulkan->instance, NULL, &vulkan->surface);
    assert(success);
}

void sdl_free_resources(vulkan_t *vulkan, sdl_t *sdl) {
    SDL_Vulkan_DestroySurface(vulkan->instance, vulkan->surface, NULL);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}
