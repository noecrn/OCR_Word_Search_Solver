#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Function to load an image
SDL_Surface* load_image(const char* filename) {
    SDL_Surface* image = IMG_Load(filename);
    if (!image) {
        fprintf(stderr, "IMG_Load Error: %s\n", IMG_GetError());
        return NULL;
    }

    return image;
}