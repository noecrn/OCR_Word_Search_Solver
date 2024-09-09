#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>

void detect_boundaries(SDL_Surface* surface);

// Function to detect letter boundaries
void detect_boundaries(SDL_Surface* surface) {
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

    // Variables to hold letter boundaries
    int min_x = width, min_y = height;
    int max_x = 0, max_y = 0;

    // Iterate through each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[y * (surface->pitch / sizeof(Uint32)) + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            // Check for black pixels (assuming black is (0,0,0) in ABGR format)
            if (r == 0 && g == 0 && b == 0) {
                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;
                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;
            }
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    // The detected boundary box
    printf("Letter boundaries: (%d, %d) to (%d, %d)\n", min_x, min_y, max_x, max_y);
}