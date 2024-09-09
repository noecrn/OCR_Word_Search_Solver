#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>

// Déclaration de la fonction
Uint32** loadImageToMatrix(const char* filename, int* width, int* height);

// Implémentation de la fonction
Uint32** loadImageToMatrix(const char* filename, int* width, int* height) {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    // Initialisation de SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        SDL_Quit();
        return NULL;
    }

    // Chargement de l'image
    SDL_Surface* imageSurface = IMG_Load(filename);
    if (!imageSurface) {
        fprintf(stderr, "Unable to load image %s! IMG_Error: %s\n", filename, IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return NULL;
    }

    // Obtenir les dimensions de l'image
    *width = imageSurface->w;
    *height = imageSurface->h;

    // Allocation de la matrice de pixels
    Uint32** matrix = (Uint32**)malloc(*height * sizeof(Uint32*));
    for (int i = 0; i < *height; ++i) {
        matrix[i] = (Uint32*)malloc(*width * sizeof(Uint32));
    }

    // Copie des pixels de l'image dans la matrice
    Uint32* pixels = (Uint32*)imageSurface->pixels;
    for (int y = 0; y < *height; ++y) {
        for (int x = 0; x < *width; ++x) {
            matrix[y][x] = pixels[(y * imageSurface->w) + x];
        }
    }

    // Libération des ressources SDL
    SDL_FreeSurface(imageSurface);
    IMG_Quit();
    SDL_Quit();

    return matrix;
}