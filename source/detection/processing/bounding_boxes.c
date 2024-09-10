#include <SDL.h>
#include <stdio.h>

#define BLOCK_SIZE 16        // Taille de chaque bloc (10x10 pixels)
#define BLACK_TOLERANCE 20   // Tolérance pour la détection des pixels noirs
#define WHITE_THRESHOLD 5    // Seuil maximum de pixels noirs pour un bloc blanc
#define SPACE_THRESHOLD 3    // Nombre maximal de blocs blancs d'affilée

int is_black_pixel(Uint8 r, Uint8 g, Uint8 b);
int count_black_pixels_in_block(SDL_Surface* surface, int start_x, int start_y);
void detect_letter_grid(SDL_Surface* surface, int* left_bound, int* right_bound, int* top_bound, int* bottom_bound);

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
    return count_black;
}

// Fonction pour propager à partir du centre et détecter la matrice de lettres
void detect_letter_grid(SDL_Surface* surface, int* left_bound, int* right_bound, int* top_bound, int* bottom_bound) {
    int center_x = surface->w / 2;
    int center_y = surface->h / 2;

    *left_bound = center_x;
    *right_bound = center_x;
    *top_bound = center_y;
    *bottom_bound = center_y;

    int consecutive_white_blocks = 0;

    // Propagation vers la gauche
    for (int x = center_x; x >= 0; x -= BLOCK_SIZE) {
        int black_pixel_count = count_black_pixels_in_block(surface, x, center_y);

        if (black_pixel_count > WHITE_THRESHOLD) {
            consecutive_white_blocks = 0;  // Reset if we found a letter
            *left_bound = x;
        } else {
            consecutive_white_blocks++;
            if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
        }
    }

    // Propagation vers la droite
    consecutive_white_blocks = 0;
    for (int x = center_x; x < surface->w; x += BLOCK_SIZE) {
        int black_pixel_count = count_black_pixels_in_block(surface, x, center_y);

        if (black_pixel_count > WHITE_THRESHOLD) {
            consecutive_white_blocks = 0;
            *right_bound = x;
        } else {
            consecutive_white_blocks++;
            if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
        }
    }

    // Propagation vers le haut
    consecutive_white_blocks = 0;
    for (int y = center_y; y >= 0; y -= BLOCK_SIZE) {
        int black_pixel_count = count_black_pixels_in_block(surface, center_x, y);

        if (black_pixel_count > WHITE_THRESHOLD) {
            consecutive_white_blocks = 0;
            *top_bound = y;
        } else {
            consecutive_white_blocks++;
            if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
        }
    }

    // Propagation vers le bas
    consecutive_white_blocks = 0;
    for (int y = center_y; y < surface->h; y += BLOCK_SIZE) {
        int black_pixel_count = count_black_pixels_in_block(surface, center_x, y);

        if (black_pixel_count > WHITE_THRESHOLD) {
            consecutive_white_blocks = 0;
            *bottom_bound = y;
        } else {
            consecutive_white_blocks++;
            if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
        }
    }

    // Affiche les bordures détectées de la grille
    printf("Grille détectée: Gauche = %d, Droite = %d, Haut = %d, Bas = %d\n",
           *left_bound, *right_bound, *top_bound, *bottom_bound);
}