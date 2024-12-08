#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <SDL2/SDL.h>

// Initialize SDL and SDL_image subsystems
int init_sdl_and_img(void);

// Clean up SDL and SDL_image subsystems
void cleanup_sdl_and_img(void);

SDL_Surface *load_image(const char *path);
void cleanup_image_loader(void);

#endif // IMAGE_LOADER_H