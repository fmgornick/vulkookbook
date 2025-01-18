#ifndef SDL_H
#define SDL_H

#include "vulkan.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

void sdl_create_window(vulkan_t *vulkan, sdl_t *sdl);
void sdl_free_resources(vulkan_t *vulkan, sdl_t *sdl);

#endif // SDL_H
