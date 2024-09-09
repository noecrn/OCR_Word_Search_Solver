#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>

// Function to detect letter boundaries and return them via pointers
void detect_boundaries(SDL_Surface* surface, int* min_x, int* min_y, int* max_x, int* max_y) {
    if (!surface || !surface->pixels) {
        fprintf(stderr, "Invalid surface or pixels\n");
        return;
    }

    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) < 0) {
            fprintf(stderr, "SDL_LockSurface Error: %s\n", SDL_GetError());
            return;
        }
    }

    int width = surface->w;
    int height = surface->h;
    Uint32* pixels = (Uint32*)surface->pixels;

    // Initialize boundaries to extreme values
    *min_x = width;
    *min_y = height;
    *max_x = 0;
    *max_y = 0;

    // Iterate through each pixel to find black pixel boundaries
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * (surface->pitch / sizeof(Uint32)) + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            // Check for black pixels (assuming black is (0,0,0))
            if (r == 0 && g == 0 && b == 0) {
                if (x < *min_x) *min_x = x;
                if (x > *max_x) *max_x = x;
                if (y < *min_y) *min_y = y;
                if (y > *max_y) *max_y = y;
            }
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    // Optionally, you could print the boundaries here
    printf("Letter boundaries: (%d, %d) to (%d, %d)\n", *min_x, *min_y, *max_x, *max_y);
}