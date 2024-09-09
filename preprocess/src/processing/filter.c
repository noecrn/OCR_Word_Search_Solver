#include <SDL2/SDL.h>
#include <stdio.h>


// Fonction pour appliquer un filtre noir/blanc
void apply_black_and_white_filter(SDL_Surface* surface) {
    if (surface == NULL) {
        printf("Surface nulle, impossible d'appliquer le filtre.\n");
        return;
    }
    printf("Application du filtre noir et blanc...\n");

    // Parcourir chaque pixel de l'image
    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            // Obtenir la couleur du pixel actuel
            Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
            
            // Décomposer le pixel en canaux rouge, vert et bleu
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Convertir en niveaux de gris (moyenne des trois composantes)
            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

            // Appliquer un seuil binaire (128 dans cet exemple)
            Uint8 bw_color = (gray > 128) ? 255 : 0;

            // Créer une nouvelle couleur en noir ou blanc
            Uint32 new_pixel = SDL_MapRGB(surface->format, bw_color, bw_color, bw_color);

            // Assigner la nouvelle couleur au pixel
            ((Uint32*)surface->pixels)[y * surface->w + x] = new_pixel;
        }
    }
    printf("Filtre appliqué avec succès.\n");
}
