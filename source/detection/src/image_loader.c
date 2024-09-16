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

    // if (!is_binary_image(image)) {
    //     fprintf(stderr, "The image is not black and white\n");
    //     SDL_FreeSurface(image);
    //     return NULL;
    // }

    return image;
}