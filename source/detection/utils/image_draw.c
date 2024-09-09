#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Function to draw a line on a surface
void draw_line(SDL_Surface *surface, int x, int y, SDL_Color color) {
    // Check if the surface is valid
    if (!surface) {
        printf("Erreur: surface non valide\n");
        return;
    }

    // Convert the color to a pixel value
    Uint32 pixelColor = SDL_MapRGB(surface->format, color.r, color.g, color.b);

    // If x == -1, draw a horizontal line on the whole width at position y
    if (x == -1) {
        if (y >= 0 && y < surface->h) {
            for (int i = 0; i < surface->w; i++) {
                ((Uint32*)surface->pixels)[y * surface->w + i] = pixelColor;
            }
        }
    }

    // If y == -1, draw a vertical line on the whole height at position x
    else if (y == -1) {
        if (x >= 0 && x < surface->w) {
            for (int i = 0; i < surface->h; i++) {
                ((Uint32*)surface->pixels)[i * surface->w + x] = pixelColor;
            }
        }
    }

    // If x and y are valid, draw a pixel at position (x, y)
    else {
        if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
            ((Uint32*)surface->pixels)[y * surface->w + x] = pixelColor;
        }
    }
}