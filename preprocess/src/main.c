#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Fonction pour vérifier si un pixel est noir
int is_black_pixel(Uint8 r, Uint8 g, Uint8 b) {
    return (r == 0 && g == 0 && b == 0);
}


// Fonction flood-fill modifiée pour explorer les groupes de pixels noirs
int flood_fill(SDL_Surface* surface, int x, int y, Uint8* visited, Uint32 white_pixel, int* pixel_coords, int* pixel_count) {
    int width = surface->w;
    int height = surface->h;
    int group_size = 0;

    // Créer une pile pour gérer les pixels à explorer
    int stack_size = width * height;
    int* stack = (int*)malloc(stack_size * 2 * sizeof(int));
    int top = -1;

    // Ajouter le pixel initial à la pile
    stack[++top] = x;
    stack[++top] = y;

    while (top >= 0) {
        int cy = stack[top--];
        int cx = stack[top--];

        // Si le pixel est hors limites, continuer
        if (cx < 0 || cy < 0 || cx >= width || cy >= height) continue;

        // Si le pixel a déjà été visité, continuer
        if (visited[cy * width + cx]) continue;

        // Marquer le pixel comme visité
        visited[cy * width + cx] = 1;

        // Récupérer le pixel actuel
        Uint32 pixel = ((Uint32*)surface->pixels)[cy * width + cx];
        Uint8 r, g, b;
        SDL_GetRGB(pixel, surface->format, &r, &g, &b);

        // Si ce n'est pas un pixel noir, continuer
        if (!is_black_pixel(r, g, b)) continue;

        // Ajouter ce pixel à la liste des pixels du groupe
        pixel_coords[group_size * 2] = cx;
        pixel_coords[group_size * 2 + 1] = cy;
        group_size++;

        // Ajouter les voisins à explorer (haut, bas, gauche, droite)
        stack[++top] = cx - 1;
        stack[++top] = cy;
        stack[++top] = cx + 1;
        stack[++top] = cy;
        stack[++top] = cx;
        stack[++top] = cy - 1;
        stack[++top] = cx;
        stack[++top] = cy + 1;
    }

    free(stack);
    *pixel_count = group_size;
    return group_size;
}

// Fonction pour nettoyer les grands groupes de pixels noirs
void clean_large_black_groups(SDL_Surface* surface, int size_threshold) {
    int width = surface->w;
    int height = surface->h;

    // Créer un pixel blanc pour remplacer les gros groupes de pixels noirs
    Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);

    // Créer un tableau pour suivre les pixels visités
    Uint8* visited = (Uint8*)calloc(width * height, sizeof(Uint8));

    // Liste temporaire pour stocker les coordonnées des pixels d'un groupe
    int* pixel_coords = (int*)malloc(width * height * 2 * sizeof(int));

    // Parcourir chaque pixel
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Si le pixel a déjà été visité, passer au suivant
            if (visited[y * width + x]) continue;

            Uint32 pixel = ((Uint32*)surface->pixels)[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Si c'est un pixel noir, explorer le groupe
            if (is_black_pixel(r, g, b)) {
                int pixel_count = 0;
                int group_size = flood_fill(surface, x, y, visited, white_pixel, pixel_coords, &pixel_count);

                // Si la taille du groupe dépasse le seuil, le supprimer (mettre en blanc)
                if (group_size > size_threshold) {
                    for (int i = 0; i < group_size; ++i) {
                        int px = pixel_coords[i * 2];
                        int py = pixel_coords[i * 2 + 1];
                        ((Uint32*)surface->pixels)[py * width + px] = white_pixel;
                    }
                }
            }
        }
    }

    free(visited);
    free(pixel_coords);
}

// Fonction pour nettoyer les artefacts noirs isolés en les remplaçant par du blanc
void clean_image(SDL_Surface* surface, int threshold) {
    if (surface == NULL) {
        printf("Surface nulle, impossible de nettoyer l'image.\n");
        return;
    }

    int width = surface->w;
    int height = surface->h;

    // Copie de la surface pour éviter d'altérer les pixels pendant la vérification
    SDL_Surface* temp_surface = SDL_ConvertSurface(surface, surface->format, 0);
    if (temp_surface == NULL) {
        printf("Erreur lors de la création de la surface temporaire: %s\n", SDL_GetError());
        return;
    }

    // Verrouiller la surface si nécessaire
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
        SDL_LockSurface(temp_surface);
    }

    // Parcourir chaque pixel de l'image (sauf les bords)
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            // Obtenir la couleur du pixel actuel
            Uint32 pixel = ((Uint32*)temp_surface->pixels)[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, temp_surface->format, &r, &g, &b, &a);

            // Si le pixel est noir, vérifier les voisins
            if (is_black_pixel(r, g, b)) {
                int white_neighbors = 0;

                // Vérifier les 8 voisins
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue; // Ignorer le pixel central

                        Uint32 neighbor_pixel = ((Uint32*)temp_surface->pixels)[(y + dy) * width + (x + dx)];
                        Uint8 nr, ng, nb, na;
                        SDL_GetRGBA(neighbor_pixel, temp_surface->format, &nr, &ng, &nb, &na);

                        if (!is_black_pixel(nr, ng, nb)) {
                            white_neighbors++;
                        }
                    }
                }

                // Si le pixel noir a trop de voisins blancs, le remplacer par blanc
                if (white_neighbors >= threshold) {
                    Uint32 white_pixel = SDL_MapRGBA(surface->format, 255, 255, 255, a);
                    ((Uint32*)surface->pixels)[y * width + x] = white_pixel;
                }
            }
        }
    }

    // Déverrouiller la surface si nécessaire
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
        SDL_UnlockSurface(temp_surface);
    }

    SDL_FreeSurface(temp_surface);
}



void apply_black_and_white_filter(SDL_Surface* surface, Uint8 threshold) {
    if (surface == NULL) {
        printf("Surface nulle, impossible d'appliquer le filtre.\n");
        return;
    }

    // Parcourir chaque pixel de l'image
    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            // Obtenir la couleur du pixel actuel
            Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
            
            // Décomposer le pixel en canaux rouge, vert et bleu
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Convertir en niveaux de gris (moyenne pondérée)
            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

            // Appliquer un seuil ajusté (passé en paramètre)
            Uint8 bw_color = (gray > threshold) ? 255 : 0;

            // Créer une nouvelle couleur en noir ou blanc
            Uint32 new_pixel = SDL_MapRGB(surface->format, bw_color, bw_color, bw_color);

            // Assigner la nouvelle couleur au pixel
            ((Uint32*)surface->pixels)[y * surface->w + x] = new_pixel;
        }
    }
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
    if(strcmp(argv[1], "../../../level_2_image_1.png") == 0)
    {
        apply_black_and_white_filter(image,150);
    }
    else
    apply_black_and_white_filter(image,180);
    
    clean_large_black_groups(image,150);  

    // Nettoyer l'image (paramètre de seuil = 6 voisins blancs ou plus)
    clean_image(image,4);

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