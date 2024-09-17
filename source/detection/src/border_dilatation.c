#include <SDL.h>
#include "../include/bounding_boxes.h"

#define BLACK_TOLERANCE 1   // Tolerance for detecting black pixels
#define DILATION 10         // Dilation amount for borders

// Function to check if a pixel is considered black
int has_black_pixels_on_line(SDL_Surface* surface, int y, int left, int right) {
    // Go through the pixels on the line between the left and right borders
    for (int x = left; x <= right; x++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

        if (is_black_pixel(r, g, b, BLACK_TOLERANCE)) {
            return 1; // Black pixel found
        }
    }
    return 0; // No black pixel found
}

// Function to check if there are black pixels in a column
int has_black_pixels_on_column(SDL_Surface* surface, int x, int top, int bottom) {
    // Go through the pixels on the column between the top and bottom borders
    for (int y = top; y <= bottom; y++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

        if (is_black_pixel(r, g, b, BLACK_TOLERANCE)) {
            return 1; // Black pixel found
        }
    }
    return 0; // No black pixel found
}

// Function to check and dilate the borders of the bounding box
void check_and_dilate_borders(SDL_Surface* surface, int* left, int* right, int* top, int* bottom) {
    int width = surface->w;
    int height = surface->h;

    // Check the top border
    if (has_black_pixels_on_line(surface, *top, *left, *right)) {
        *top = (*top - DILATION > 0) ? *top - DILATION : 1;
    }

    // Check the bottom border
    if (has_black_pixels_on_line(surface, *bottom, *left, *right)) {
        *bottom = (*bottom + DILATION < height) ? *bottom + DILATION : height - 1;
    }

    // Check the left border
    if (has_black_pixels_on_column(surface, *left, *top, *bottom)) {
        *left = (*left - DILATION > 0) ? *left - DILATION : 1;
    }

    // Check the right border
    if (has_black_pixels_on_column(surface, *right, *top, *bottom)) {
        *right = (*right + DILATION < width) ? *right + DILATION : width - 1;
    }
}