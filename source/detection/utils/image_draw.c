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

// Function to draw the grid on an SDL surface
void draw_grid(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound, int num_rows, int num_cols) {
    SDL_Color blue = {0, 0, 255, 255}; // Blue color for the grid lines

    int block_width = (right_bound - left_bound) / num_cols;
    int block_height = (bottom_bound - top_bound) / num_rows;

    // Draw vertical lines
    for (int col = 0; col <= num_cols; col++) {
        int x = left_bound + col * block_width;
        for (int y = top_bound; y <= bottom_bound; y++) {
            Uint32* pixels = (Uint32*)surface->pixels;
            pixels[(y * surface->pitch / 4) + x] = SDL_MapRGBA(surface->format, blue.r, blue.g, blue.b, blue.a);
        }
    }

    // Draw horizontal lines
    for (int row = 0; row <= num_rows; row++) {
        int y = top_bound + row * block_height;
        for (int x = left_bound; x <= right_bound; x++) {
            Uint32* pixels = (Uint32*)surface->pixels;
            pixels[(y * surface->pitch / 4) + x] = SDL_MapRGBA(surface->format, blue.r, blue.g, blue.b, blue.a);
        }
    }
}