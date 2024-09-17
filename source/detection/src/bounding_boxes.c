#include <SDL.h>
#include <stdio.h>
#include "../include/rendering.h"

// Function to check if a pixel is black based on a tolerance value
int is_black_pixel(Uint8 r, Uint8 g, Uint8 b, int black_tolerance) {
    return r < black_tolerance && g < black_tolerance && b < black_tolerance;
}

// Fonction pour compter les pixels noirs dans un bloc de taille BLOCK_SIZE x BLOCK_SIZE
int count_black_pixels_in_block(SDL_Surface* surface, int start_x, int start_y, int block_size, int black_tolerance) {
    int count_black = 0;

    // Limite le bloc à l'intérieur des dimensions de l'image
    for (int y = start_y; y < start_y + block_size && y < surface->h; y++) {
        for (int x = start_x; x < start_x + block_size && x < surface->w; x++) {
            Uint32 pixel = 0;
            Uint8 r, g, b, a;
            pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            if (is_black_pixel(r, g, b, black_tolerance)) {
                count_black++;
            }
        }
    }

    // Draw the border around the block
    // draw_square(surface, start_x, start_y, BLOCK_SIZE, (SDL_Color){0, 0, 255, 255});

    return count_black;
}

// Fonction pour propager à partir du centre et détecter la matrice de lettres
void detect_letter_grid(
    SDL_Surface* surface, 
    int* left_bound, 
    int* right_bound, 
    int* top_bound, 
    int* bottom_bound, 
    int block_size, 
    int black_tolerance, 
    int white_threshold, 
    int space_threshold
) {
    // Taille de l'image
    int width = surface->w;
    int height = surface->h;

    // Initialisation des bordures avec les dimensions de l'image
    *left_bound = width;
    *right_bound = 0;
    *top_bound = height;
    *bottom_bound = 0;

    // On peut démarrer la recherche à partir de différents points dans l'image pour capturer toutes les lettres
    int start_points[][2] = {
        {width / 2, height / 2}, 
        {width / 2 + block_size, height / 2 + block_size}, 
        {width / 2 - block_size, height / 2 + block_size}, 
        {width / 2 + block_size, height / 2 - block_size}, 
        {width / 2 - block_size, height / 2 - block_size}
    };

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers la gauche
        for (int x = start_x; x >= 0; x -= block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) {
                consecutive_white_blocks = 0;
                if (x - block_size >= 0) {
                    *left_bound = x < *left_bound ? x : *left_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers la droite
        consecutive_white_blocks = 0;
        for (int x = start_x; x < width; x += block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) {
                consecutive_white_blocks = 0;
                if (x + block_size < width) {
                    *right_bound = x + block_size > *right_bound ? x + block_size : *right_bound;
                } else {
                    *right_bound = x > *right_bound ? x : *right_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers le haut
        consecutive_white_blocks = 0;
        for (int y = start_y; y >= 0; y -= block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) {
                consecutive_white_blocks = 0;
                if (y - block_size >= 0) {
                    *top_bound = y < *top_bound ? y : *top_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers le bas
        consecutive_white_blocks = 0;
        for (int y = start_y; y < height; y += block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) {
                consecutive_white_blocks = 0;
                if (y + block_size < height) {
                    *bottom_bound = y + block_size > *bottom_bound ? y + block_size : *bottom_bound;
                } else {
                    *bottom_bound = y > *bottom_bound ? y : *bottom_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break;
            }
        }
    }
}