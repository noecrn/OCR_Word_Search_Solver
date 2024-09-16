#include <SDL.h>
#include "../include/bounding_boxes.h"

// Définition de la tolérance pour considérer un pixel comme noir
#define BLACK_TOLERANCE 1
#define DILATION 10 // Dilation à appliquer aux bordures si des pixels noirs sont trouvés

// Fonction pour vérifier la présence de pixels noirs sur une ligne horizontale
int has_black_pixels_on_line(SDL_Surface* surface, int y, int left, int right) {
    // Parcourir les pixels sur la ligne entre les bordures left et right
    for (int x = left; x <= right; x++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

        if (is_black_pixel(r, g, b)) {
            return 1; // Pixel noir trouvé
        }
    }
    return 0; // Aucun pixel noir
}

// Fonction pour vérifier la présence de pixels noirs sur une ligne verticale
int has_black_pixels_on_column(SDL_Surface* surface, int x, int top, int bottom) {
    // Parcourir les pixels sur la colonne entre les bordures top et bottom
    for (int y = top; y <= bottom; y++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

        if (is_black_pixel(r, g, b)) {
            return 1; // Pixel noir trouvé
        }
    }
    return 0; // Aucun pixel noir
}

// Fonction pour vérifier les bordures et dilater si des pixels noirs sont trouvés
void check_and_dilate_borders(SDL_Surface* surface, int* left, int* right, int* top, int* bottom) {
    int width = surface->w;
    int height = surface->h;

    // Vérifier la bordure supérieure (top)
    if (has_black_pixels_on_line(surface, *top, *left, *right)) {
        *top = (*top - DILATION > 0) ? *top - DILATION : 1;
    }

    // Vérifier la bordure inférieure (bottom)
    if (has_black_pixels_on_line(surface, *bottom, *left, *right)) {
        *bottom = (*bottom + DILATION < height) ? *bottom + DILATION : height - 1;
    }

    // Vérifier la bordure gauche (left)
    if (has_black_pixels_on_column(surface, *left, *top, *bottom)) {
        *left = (*left - DILATION > 0) ? *left - DILATION : 1;
    }

    // Vérifier la bordure droite (right)
    if (has_black_pixels_on_column(surface, *right, *top, *bottom)) {
        *right = (*right + DILATION < width) ? *right + DILATION : width - 1;
    }
}