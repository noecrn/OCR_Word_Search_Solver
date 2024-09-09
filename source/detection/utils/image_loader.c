#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Function to check if the image is binary (black and white)
// int is_binary_image(SDL_Surface* image) {
//     Uint32 black = SDL_MapRGB(image->format, 0, 0, 0);
//     Uint32 white = SDL_MapRGB(image->format, 255, 255, 255);

//     for (int y = 0; y < image->h; ++y) {
//         for (int x = 0; x < image->w; ++x) {
//             Uint32 pixel = ((Uint32*)image->pixels)[y * image->pitch / 4 + x];
//             if (pixel != black && pixel != white) {
//                 return 0; // false
//             }
//         }
//     }
//     return 1; // true
// }

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