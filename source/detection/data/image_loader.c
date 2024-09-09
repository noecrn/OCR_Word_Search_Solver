#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>

Uint32** loadImageToMatrix(const char* filename, int* width, int* height);

// Load an image into a matrix of pixels
Uint32** loadImageToMatrix(const char* filename, int* width, int* height) {
    // SDL initialization
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    // SDL_image initialization
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        SDL_Quit();
        return NULL;
    }

    // Image loading
    SDL_Surface* imageSurface = IMG_Load(filename);
    if (!imageSurface) {
        fprintf(stderr, "Unable to load image %s! IMG_Error: %s\n", filename, IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return NULL;
    }

    // Get image dimensions
    *width = imageSurface->w;
    *height = imageSurface->h;

    // Allocate memory for the matrix
    Uint32** matrix = (Uint32**)malloc(*height * sizeof(Uint32*));
    for (int i = 0; i < *height; ++i) {
        matrix[i] = (Uint32*)malloc(*width * sizeof(Uint32));
    }

    // Copy image pixels to the matrix
    Uint32* pixels = (Uint32*)imageSurface->pixels;
    for (int y = 0; y < *height; ++y) {
        for (int x = 0; x < *width; ++x) {
            matrix[y][x] = pixels[(y * imageSurface->w) + x];
        }
    }

    // Liberate resources
    SDL_FreeSurface(imageSurface);
    IMG_Quit();
    SDL_Quit();

    return matrix;
}