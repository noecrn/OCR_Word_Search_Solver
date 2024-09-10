#include <SDL.h>
#include <stdio.h>

#include "bounding_boxes.h"
#include "../utils/image_draw.h"

#define BLOCK_SIZE 5        // Taille de chaque bloc (10x10 pixels)
#define BLACK_TOLERANCE 1   // Tolérance pour la détection des pixels noirs
#define WHITE_THRESHOLD 0    // Seuil maximum de pixels noirs pour un bloc blanc
#define SPACE_THRESHOLD 0    // Nombre maximal de blocs blancs d'affilée

// Fonction modifiée pour détecter le nombre de colonnes
int detect_columns(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound) {
    int column_count = 0;                 // Nombre de colonnes détectées
    int in_black_column = 0;              // Indicateur pour savoir si on est dans une colonne avec des pixels noirs

    // Parcourt de gauche à droite sur les colonnes
    for (int x = left_bound; x <= right_bound; x += BLOCK_SIZE) {
        int black_pixel_count = 0;

        // Compte le nombre de pixels noirs dans cette colonne
        for (int y = top_bound; y <= bottom_bound; y += BLOCK_SIZE) {
            black_pixel_count += count_black_pixels_in_block(surface, x, y);
        }

        // Si cette colonne contient des pixels noirs
        if (black_pixel_count > WHITE_THRESHOLD) {
            if (!in_black_column) {
                // Si on n'était pas déjà dans une colonne noire, on en compte une nouvelle
                column_count++;
                in_black_column = 1;  // On est maintenant dans une colonne noire
            }
        } else {
            // Si la colonne est blanche (peu ou pas de pixels noirs)
            in_black_column = 0;  // On sort d'une colonne noire
        }
    }

    return column_count;
}

// Fonction modifiée pour détecter le nombre de lignes
int detect_rows(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound) {
    int row_count = 0;                  // Nombre de lignes détectées
    int in_black_row = 0;               // Indicateur pour savoir si on est dans une ligne avec des pixels noirs

    // Parcourt de haut en bas sur les lignes
    for (int y = top_bound; y <= bottom_bound; y += BLOCK_SIZE) {
        int black_pixel_count = 0;

        // Compte le nombre de pixels noirs dans cette ligne
        for (int x = left_bound; x <= right_bound; x += BLOCK_SIZE) {
            black_pixel_count += count_black_pixels_in_block(surface, x, y);
        }

        // Si cette ligne contient des pixels noirs
        if (black_pixel_count > WHITE_THRESHOLD) {
            if (!in_black_row) {
                // Si on n'était pas déjà dans une ligne noire, on en compte une nouvelle
                row_count++;
                in_black_row = 1;  // On est maintenant dans une ligne noire
            }
        } else {
            // Si la ligne est blanche (peu ou pas de pixels noirs)
            in_black_row = 0;  // On sort d'une ligne noire
        }
    }

    return row_count;
}

// Fonction principale pour analyser la grille avec les nouvelles règles
void analyze_grid(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound, int* column_count, int* row_count) {
    // Détecter le nombre de colonnes avec les nouvelles règles
    *column_count = detect_columns(surface, left_bound, right_bound, top_bound, bottom_bound);

    // Détecter le nombre de lignes avec les nouvelles règles
    *row_count = detect_rows(surface, left_bound, right_bound, top_bound, bottom_bound);
}