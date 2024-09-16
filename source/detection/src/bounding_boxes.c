#include <SDL.h>
#include <stdio.h>
#include "../include/rendering.h"

#define BLOCK_SIZE 15        // Taille de chaque bloc (10x10 pixels)
#define BLACK_TOLERANCE 1   // Tolérance pour la détection des pixels noirs
#define WHITE_THRESHOLD 0    // Seuil maximum de pixels noirs pour un bloc blanc
#define SPACE_THRESHOLD 3    // Nombre maximal de blocs blancs d'affilée

// Function to check if a pixel is black based on a tolerance value
int is_black_pixel(Uint8 r, Uint8 g, Uint8 b) {
    return r < BLACK_TOLERANCE && g < BLACK_TOLERANCE && b < BLACK_TOLERANCE;
}

// Fonction pour compter les pixels noirs dans un bloc de taille BLOCK_SIZE x BLOCK_SIZE
int count_black_pixels_in_block(SDL_Surface* surface, int start_x, int start_y) {
    int count_black = 0;

    // Limite le bloc à l'intérieur des dimensions de l'image
    for (int y = start_y; y < start_y + BLOCK_SIZE && y < surface->h; y++) {
        for (int x = start_x; x < start_x + BLOCK_SIZE && x < surface->w; x++) {
            Uint32 pixel = 0;
            Uint8 r, g, b, a;
            pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            if (is_black_pixel(r, g, b)) {
                count_black++;
            }
        }
    }

    // Draw the border around the block
    // draw_square(surface, start_x, start_y, BLOCK_SIZE, (SDL_Color){0, 0, 255, 255});

    return count_black;
}

// Fonction pour propager à partir du centre et détecter la matrice de lettres
void detect_letter_grid(SDL_Surface* surface, int* left_bound, int* right_bound, int* top_bound, int* bottom_bound) {
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
        {width / 2 + BLOCK_SIZE, height / 2 + BLOCK_SIZE}, 
        {width / 2 - BLOCK_SIZE, height / 2 + BLOCK_SIZE}, 
        {width / 2 + BLOCK_SIZE, height / 2 - BLOCK_SIZE}, 
        {width / 2 - BLOCK_SIZE, height / 2 - BLOCK_SIZE}
    };

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers la gauche
        for (int x = start_x; x >= 0; x -= BLOCK_SIZE) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y);

            if (black_pixel_count > WHITE_THRESHOLD) {
                consecutive_white_blocks = 0;
                if (x - BLOCK_SIZE >= 0) {
                    *left_bound = x < *left_bound ? x : *left_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers la droite
        consecutive_white_blocks = 0;
        for (int x = start_x; x < width; x += BLOCK_SIZE) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y);

            if (black_pixel_count > WHITE_THRESHOLD) {
                consecutive_white_blocks = 0;
                if (x + BLOCK_SIZE < width) {
                    *right_bound = x + BLOCK_SIZE > *right_bound ? x + BLOCK_SIZE : *right_bound;
                } else {
                    *right_bound = x > *right_bound ? x : *right_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers le haut
        consecutive_white_blocks = 0;
        for (int y = start_y; y >= 0; y -= BLOCK_SIZE) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y);

            if (black_pixel_count > WHITE_THRESHOLD) {
                consecutive_white_blocks = 0;
                if (y - BLOCK_SIZE >= 0) {
                    *top_bound = y < *top_bound ? y : *top_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        // Propagation vers le bas
        consecutive_white_blocks = 0;
        for (int y = start_y; y < height; y += BLOCK_SIZE) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y);

            if (black_pixel_count > WHITE_THRESHOLD) {
                consecutive_white_blocks = 0;
                if (y + BLOCK_SIZE < height) {
                    *bottom_bound = y + BLOCK_SIZE > *bottom_bound ? y + BLOCK_SIZE : *bottom_bound;
                } else {
                    *bottom_bound = y > *bottom_bound ? y : *bottom_bound;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
            }
        }
    }
}