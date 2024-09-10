#include <SDL.h>
#include <stdio.h>

#define BLOCK_SIZE 10        // Taille de chaque bloc (10x10 pixels)
#define BLACK_TOLERANCE 1   // Tolérance pour la détection des pixels noirs
#define WHITE_THRESHOLD 0    // Seuil maximum de pixels noirs pour un bloc blanc
#define SPACE_THRESHOLD 4    // Nombre maximal de blocs blancs d'affilée

// Function to check if a pixel is black based on a tolerance value
int is_black_pixel(Uint8 r, Uint8 g, Uint8 b) {
    return r < BLACK_TOLERANCE && g < BLACK_TOLERANCE && b < BLACK_TOLERANCE;
}

// Fonction pour compter les pixels noirs dans un bloc de taille BLOCK_SIZE x BLOCK_SIZE
int count_black_pixels_in_block(SDL_Surface* surface, int start_x, int start_y) {
    int count_black = 0;
    Uint32 red_pixel = SDL_MapRGBA(surface->format, 255, 0, 0, 255); // Rouge

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

            // Dessiner les bords en rouge
            if (x == start_x || x == start_x + BLOCK_SIZE - 1 || y == start_y || y == start_y + BLOCK_SIZE - 1) {
                ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x] = red_pixel;
            }
        }
    }
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

        // Propagation vers la droite
        consecutive_white_blocks = 0;
        for (int x = start_x; x < width; x += BLOCK_SIZE) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y);

            if (black_pixel_count > WHITE_THRESHOLD) {
                consecutive_white_blocks = 0;
                if (x + BLOCK_SIZE < width) {
                    *right_bound = x + BLOCK_SIZE;
                } else {
                    *right_bound = x;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
            }
        }

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

        // Propagation vers le bas
        consecutive_white_blocks = 0;
        for (int y = start_y; y < height; y += BLOCK_SIZE) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y);

            if (black_pixel_count > WHITE_THRESHOLD) {
                consecutive_white_blocks = 0;
                if (y + BLOCK_SIZE < height) {
                    *bottom_bound = y + BLOCK_SIZE;
                } else {
                    *bottom_bound = y;
                }
            } else {
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= SPACE_THRESHOLD) break;
            }
        }
    }

    // Affichage des bordures détectées
    printf("Grille détectée: Gauche = %d, Droite = %d, Haut = %d, Bas = %d\n",
           *left_bound, *right_bound, *top_bound, *bottom_bound);
}

// // Fonction pour analyser la grille et déterminer le nombre de lignes et de colonnes
// void analyze_grid(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound, int* num_rows, int* num_cols) {
//     *num_rows = 0;
//     *num_cols = 0;

//     int column_heights[(right_bound - left_bound) / BLOCK_SIZE + 1];
//     int column_index = 0;

//     // Compter les colonnes
//     for (int x = left_bound; x <= right_bound; x += BLOCK_SIZE) {
//         int consecutive_white_blocks = 0;
//         int column_height = 0;
//         for (int y = top_bound; y <= bottom_bound; y += BLOCK_SIZE) {
//             int black_pixel_count = count_black_pixels_in_block(surface, x, y);

//             if (black_pixel_count <= WHITE_THRESHOLD) {
//                 consecutive_white_blocks++;
//                 if (consecutive_white_blocks >= SPACE_THRESHOLD) {
//                     break;
//                 }
//             } else {
//                 consecutive_white_blocks = 0;
//                 column_height++;
//             }
//         }

//         column_heights[column_index++] = column_height;

//         if (consecutive_white_blocks < SPACE_THRESHOLD) {
//             (*num_cols)++;
//         }
//     }

//     // Vérifier la taille des colonnes
//     int invalid_columns = 0;
//     for (int i = 1; i < column_index; i++) {
//         if (column_heights[i] != column_heights[0]) {
//             invalid_columns++;
//             if (invalid_columns >= 3) {
//                 *num_cols = 0;
//                 printf("Les colonnes ne sont pas valides.\n");
//                 break;
//             }
//         } else {
//             invalid_columns = 0;
//         }
//     }

//     // Compter les lignes
//     for (int y = top_bound; y <= bottom_bound; y += BLOCK_SIZE) {
//         int consecutive_white_blocks = 0;
//         for (int x = left_bound; x <= right_bound; x += BLOCK_SIZE) {
//             int black_pixel_count = count_black_pixels_in_block(surface, x, y);

//             if (black_pixel_count <= WHITE_THRESHOLD) {
//                 consecutive_white_blocks++;
//                 if (consecutive_white_blocks >= SPACE_THRESHOLD) {
//                     break;
//                 }
//             } else {
//                 consecutive_white_blocks = 0;
//             }
//         }
//         if (consecutive_white_blocks < SPACE_THRESHOLD) {
//             (*num_rows)++;
//         }
//     }

//     // Affiche le nombre de lignes et de colonnes détectées
//     printf("Nombre de lignes: %d, Nombre de colonnes: %d\n", *num_rows, *num_cols);
// }