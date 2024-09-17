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
    // draw_square(surface, start_x, start_y, block_size, (SDL_Color){0, 0, 255, 255});

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
    int width_center = width / 2;
    int height_center = height / 2 + block_size * 3;
    int start_points[][2] = {
        {width_center, height_center}, 
        {width_center + block_size, height_center + block_size}, 
        {width_center - block_size, height_center + block_size}, 
        {width_center + block_size, height_center - block_size}, 
        {width_center - block_size, height_center - block_size}
    };

    int num_start_points = sizeof(start_points) / sizeof(start_points[0]);

    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers la gauche
        for (int x = start_x; x >= 0; x -= block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance);
            printf("Left - x: %d, y: %d, black_pixel_count: %d\n", x, start_y, black_pixel_count);

            if (black_pixel_count > white_threshold) {
                consecutive_white_blocks = 0;
                if (x < *left_bound) {
                    *left_bound = x;
                    printf("Updated Left Bound: %d\n", *left_bound);
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks > space_threshold) { //! > and not >=
                    printf("Breaking left at x: %d, consecutive_white_blocks: %d\n", x, consecutive_white_blocks);
                    break;
                }
            }
        }
    }

    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers la droite
        consecutive_white_blocks = 0;
        for (int x = start_x; x < width; x += block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance);
            printf("Right - x: %d, y: %d, black_pixel_count: %d\n", x, start_y, black_pixel_count);

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

    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers le haut
        consecutive_white_blocks = 0;
        for (int y = start_y; y >= 0; y -= block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance);
            printf("Top - x: %d, y: %d, black_pixel_count: %d\n", start_x, y, black_pixel_count);

            if (black_pixel_count > white_threshold) {
                consecutive_white_blocks = 0;
                if (y < *top_bound) {
                    *top_bound = y;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break;
            }
        }
    }

    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers le bas
        consecutive_white_blocks = 0;
        for (int y = start_y; y < height; y += block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance);
            printf("Bottom - x: %d, y: %d, black_pixel_count: %d\n", start_x, y, black_pixel_count);

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

    printf("Left Bound: %d, Right Bound: %d, Top Bound: %d, Bottom Bound: %d\n", *left_bound, *right_bound, *top_bound, *bottom_bound);
}