#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

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


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <path_to_image>\n", argv[0]);
        return 1;
    }

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Initialiser SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Erreur lors de l'initialisation de SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Charger l'image avec le chemin passé en argument
    SDL_Surface* image = IMG_Load(argv[1]);

    if (image == NULL) {
        printf("Erreur lors du chargement de l'image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Appliquer le filtre noir et blanc
    apply_black_and_white_filter(image);

    // Sauvegarder l'image résultante
    if (IMG_SavePNG(image, "../../output_image.png") != 0) {
        printf("Erreur lors de la sauvegarde de l'image: %s\n", SDL_GetError());
    } else {
    printf("Image sauvegardée sous le nom output_image.bmp\n");
    }

    // Afficher l'image dans une fenêtre SDL
SDL_Window* window = SDL_CreateWindow("Image Noir et Blanc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image->w, image->h, SDL_WINDOW_SHOWN);
if (window == NULL) {
    printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
    SDL_FreeSurface(image);
    SDL_Quit();
    return 1;
}

SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
SDL_BlitSurface(image, NULL, screenSurface, NULL); // Copier l'image dans la fenêtre
SDL_UpdateWindowSurface(window); // Mettre à jour l'affichage

// Attendre 5 secondes avant de fermer
SDL_Delay(7000);

SDL_DestroyWindow(window);

    // Libérer les ressources
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();

    return 0;
}