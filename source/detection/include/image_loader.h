#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int is_binary_image(SDL_Surface *image);
SDL_Surface *load_image(const char *filename);

#endif // IMAGE_LOADER_H