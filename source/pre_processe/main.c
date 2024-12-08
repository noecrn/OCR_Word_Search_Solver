#include "data/utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Function to check if a pixel is black
int is_black_pixel(Uint8 r, Uint8 g, Uint8 b) {
  return (r == 0 && g == 0 && b == 0);
}
// Function to check if a pixel is light pink
int is_light_pink_pixel(Uint8 r, Uint8 g, Uint8 b) {
  return (r > 200 && g < 190 && b > 150 && b < 255);
}

// Flood-fill function that explores groups of black pixels
int flood_fill(SDL_Surface *surface, int x, int y, Uint8 *visited,
               int *pixel_coords, int *pixel_count) {
  int width = surface->w;
  int height = surface->h;
  int group_size = 0;

  // Create a stack to manage pixels to explore
  int stack_size = width * height;
  int *stack = (int *)malloc(stack_size * 2 * sizeof(int));
  int top = -1;

  // Add the initial pixel to the stack
  stack[++top] = x;
  stack[++top] = y;

  while (top >= 0) {
    int cy = stack[top--];
    int cx = stack[top--];

    // If the pixel has already been visited, continue
    if (cx < 0 || cy < 0 || cx >= width || cy >= height)
      continue;

    // Si le pixel a déjà été visité, continuer
    if (visited[cy * width + cx])
      continue;

    // Mark the pixel as visited
    visited[cy * width + cx] = 1;

    // current pixel
    Uint32 pixel = ((Uint32 *)surface->pixels)[cy * width + cx];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

    // If it's not a black pixel, continue
    if (!is_black_pixel(r, g, b))
      continue;

    // Add this pixel to the list of pixels in the group
    pixel_coords[group_size * 2] = cx;
    pixel_coords[group_size * 2 + 1] = cy;
    group_size++;

    // Add neighbors to explore (top, bottom, left, right)
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

// Function to clean up large groups of black pixels
void clean_large_black_groups(SDL_Surface *surface, int size_threshold) {
  int width = surface->w;
  int height = surface->h;

  // White pixel to replace large groups of black pixels
  Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);

  // Array to keep track of visited pixels
  Uint8 *visited = (Uint8 *)calloc(width * height, sizeof(Uint8));

  // Temporary list to store coordinates of pixels in a group
  int *pixel_coords = (int *)malloc(width * height * 2 * sizeof(int));

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // If the pixel has already been visited, skip to the next
      if (visited[y * width + x])
        continue;

      Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);

      // If it's a black pixel, explore the group
      if (is_black_pixel(r, g, b)) {
        int pixel_count = 0;
        int group_size =
            flood_fill(surface, x, y, visited, pixel_coords, &pixel_count);

        // If the group size exceeds the threshold, remove it (set to white)
        if (group_size > size_threshold) {
          for (int i = 0; i < group_size; ++i) {
            int px = pixel_coords[i * 2];
            int py = pixel_coords[i * 2 + 1];
            ((Uint32 *)surface->pixels)[py * width + px] = white_pixel;
          }
        }
      }
    }
  }

  free(visited);
  free(pixel_coords);
}

// Function to clean isolated black artifacts by replacing them with white
void clean_image(SDL_Surface *surface, int threshold) {
  if (surface == NULL) {
    printf("Surface nulle, impossible de nettoyer l'image.\n");
    return;
  }

  int width = surface->w;
  int height = surface->h;

  // Copy the surface to avoid altering pixels during verification
  SDL_Surface *temp_surface = SDL_ConvertSurface(surface, surface->format, 0);
  if (temp_surface == NULL) {
    printf("Erreur lors de la création de la surface temporaire: %s\n",
           SDL_GetError());
    return;
  }

  // Lock the surface if necessary
  if (SDL_MUSTLOCK(surface)) {
    SDL_LockSurface(surface);
    SDL_LockSurface(temp_surface);
  }

  // Loop through each pixel in the image (except edges)
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      // Get the color of the current pixel
      Uint32 pixel = ((Uint32 *)temp_surface->pixels)[y * width + x];
      Uint8 r, g, b, a;
      SDL_GetRGBA(pixel, temp_surface->format, &r, &g, &b, &a);

      // If the pixel is black, check the neighbors
      if (is_black_pixel(r, g, b)) {
        int white_neighbors = 0;

        // Check the 8 neighbors
        for (int dy = -1; dy <= 1; ++dy) {
          for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0)
              continue; // Ignore the central pixel

            Uint32 neighbor_pixel =
                ((Uint32 *)temp_surface->pixels)[(y + dy) * width + (x + dx)];
            Uint8 nr, ng, nb, na;
            SDL_GetRGBA(neighbor_pixel, temp_surface->format, &nr, &ng, &nb,
                        &na);

            if (!is_black_pixel(nr, ng, nb)) {
              white_neighbors++;
            }
          }
        }

        // If the black pixel has too many white neighbors, replace it with
        // white
        if (white_neighbors >= threshold) {
          Uint32 white_pixel = SDL_MapRGBA(surface->format, 255, 255, 255, a);
          ((Uint32 *)surface->pixels)[y * width + x] = white_pixel;
        }
      }
    }
  }

  // Unlock the surface if necessary
  if (SDL_MUSTLOCK(surface)) {
    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(temp_surface);
  }

  SDL_FreeSurface(temp_surface);
}

void extract(SDL_Surface *surface) {
  if (surface == NULL) {
    printf("Surface nulle, impossible d'appliquer le filtre.\n");
    return;
  }

  for (int y = 0; y < surface->h; ++y) {
    for (int x = 0; x < surface->w; ++x) {
      Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);
      if (r >= 183 && r < 202 && g >= 183 && g < 202 && b >= 183 && b < 202) {
        r = 0;
        g = 0;
        b = 0;
      }

      // Create a new black or white color
      Uint32 new_pixel = SDL_MapRGB(surface->format, r, g, b);

      // Assign the new color to the pixel
      ((Uint32 *)surface->pixels)[y * surface->w + x] = new_pixel;
    }
  }
}

void apply_black_and_white_filter(SDL_Surface *surface, Uint8 threshold) {
  if (surface == NULL) {
    printf("Surface nulle, impossible d'appliquer le filtre.\n");
    return;
  }

  // Loop through each pixel in the image
  for (int y = 0; y < surface->h; ++y) {
    for (int x = 0; x < surface->w; ++x) {
      // Get the current pixel color
      Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];

      // Decompose the pixel into red, green, and blue channels
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);

      // Check if the pixel is light pink and turn it black if it is
      if (is_light_pink_pixel(r, g, b)) {
        r = 0;
        g = 0;
        b = 0;
      }

      // Convert to grayscale (weighted average)
      Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

      // Apply an adjusted threshold (passed as a parameter)
      Uint8 bw_color = (gray > threshold) ? 255 : 0;

      // Create a new black or white color
      Uint32 new_pixel =
          SDL_MapRGB(surface->format, bw_color, bw_color, bw_color);

      // Assign the new color to the pixel
      ((Uint32 *)surface->pixels)[y * surface->w + x] = new_pixel;
    }
  }
}

void remove_drawings(SDL_Surface *surface, int size_threshold) {
  int width = surface->w;
  int height = surface->h;

  // Blanc pour remplacer les dessins
  Uint32 white_pixel = SDL_MapRGB(surface->format, 255, 255, 255);

  // Tableau pour suivre les pixels visités
  Uint8 *visited = (Uint8 *)calloc(width * height, sizeof(Uint8));
  int *pixel_coords = (int *)malloc(width * height * 2 * sizeof(int));

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (visited[y * width + x])
        continue;

      Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);

      // Si le pixel fait partie d'un dessin, effectuez un flood fill
      if (is_black_pixel(r, g, b)) {
        int pixel_count = 0;
        int group_size =
            flood_fill(surface, x, y, visited, pixel_coords, &pixel_count);

        // Si la taille du groupe est grande, remplacez-le par du blanc
        if (group_size > size_threshold) {
          for (int i = 0; i < group_size; ++i) {
            int px = pixel_coords[i * 2];
            int py = pixel_coords[i * 2 + 1];
            ((Uint32 *)surface->pixels)[py * width + px] = white_pixel;
          }
        }
      }
    }
  }

  free(visited);
  free(pixel_coords);
}

void dilate_image(SDL_Surface *surface) {
  int width = surface->w;
  int height = surface->h;

  SDL_Surface *temp_surface = SDL_ConvertSurface(surface, surface->format, 0);
  if (temp_surface == NULL) {
    printf("Erreur lors de la création de la surface temporaire: %s\n",
           SDL_GetError());
    return;
  }

  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      Uint32 pixel = ((Uint32 *)temp_surface->pixels)[y * width + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, temp_surface->format, &r, &g, &b);

      // Si un pixel est noir ou presque noir
      if (r < 50 && g < 50 && b < 50) {
        // Rendre tous les voisins noirs
        for (int dy = -1; dy <= 1; ++dy) {
          for (int dx = -1; dx <= 1; ++dx) {
            ((Uint32 *)surface->pixels)[(y + dy) * width + (x + dx)] =
                SDL_MapRGB(surface->format, 0, 0, 0);
          }
        }
      }
    }
  }

  SDL_FreeSurface(temp_surface);
}

void remove_small_black_groups(SDL_Surface *surface, int min_size) {
  int width = surface->w;
  int height = surface->h;

  // Créer un tableau pour marquer les pixels déjà visités
  int *visited = (int *)calloc(width * height, sizeof(int));
  if (visited == NULL) {
    printf("Erreur d'allocation mémoire.\n");
    return;
  }

  // Fonction pour explorer une composante connectée via un algorithme de
  // flood-fill
  void flood_fill2(int x, int y, int *group_size) {
    if (x < 0 || x >= width || y < 0 || y >= height)
      return; // Limites de l'image
    if (visited[y * width + x])
      return; // Déjà visité

    Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

    // Si ce n'est pas un pixel noir, ignorer
    if (r > 50 || g > 50 || b > 50)
      return;

    // Marquer ce pixel comme visité
    visited[y * width + x] = 1;
    (*group_size)++;

    // Explorer les voisins (8-connectivité)
    flood_fill2(x + 1, y, group_size);
    flood_fill2(x - 1, y, group_size);
    flood_fill2(x, y + 1, group_size);
    flood_fill2(x, y - 1, group_size);
    flood_fill2(x + 1, y + 1, group_size);
    flood_fill2(x - 1, y - 1, group_size);
    flood_fill2(x + 1, y - 1, group_size);
    flood_fill2(x - 1, y + 1, group_size);
  }

  // Fonction pour supprimer un groupe donné
  void clear_group(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height)
      return; // Limites de l'image
    if (!visited[y * width + x])
      return; // Pas un pixel marqué

    Uint32 pixel = ((Uint32 *)surface->pixels)[y * width + x];
    Uint8 r, g, b;
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

    if (r > 50 || g > 50 || b > 50)
      return; // Si ce n'est pas noir

    // Effacer ce pixel (le rendre blanc)
    ((Uint32 *)surface->pixels)[y * width + x] =
        SDL_MapRGB(surface->format, 255, 255, 255);

    // Dé-marquer ce pixel
    visited[y * width + x] = 0;

    // Effacer les voisins (8-connectivité)
    clear_group(x + 1, y);
    clear_group(x - 1, y);
    clear_group(x, y + 1);
    clear_group(x, y - 1);
    clear_group(x + 1, y + 1);
    clear_group(x - 1, y - 1);
    clear_group(x + 1, y - 1);
    clear_group(x - 1, y + 1);
  }

  // Parcourir tous les pixels de l'image
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (!visited[y * width + x]) {
        // Initialiser la taille du groupe
        int group_size = 0;

        // Compter la taille du groupe via flood-fill
        flood_fill2(x, y, &group_size);

        // Si le groupe est trop petit, le supprimer
        if (group_size > 0 && group_size < min_size) {
          clear_group(x, y);
        }
      }
    }
  }

  // Libérer la mémoire allouée
  free(visited);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <path_to_image>\n", argv[0]);
    return 1;
  }

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
    return 1;
  }

  // Initialize SDL_image
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    printf("Erreur lors de l'initialisation de SDL_image: %s\n",
           IMG_GetError());
    SDL_Quit();
    return 1;
  }

  // Load the image using the path provided as an argument
  SDL_Surface *image = IMG_Load(argv[1]);

  if (image == NULL) {
    printf("Erreur lors du chargement de l'image: %s\n", IMG_GetError());
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  apply_black_and_white_filter(image, 180);

  int biggest_component = find_biggest_black_component(image);

  //printf("Nombre de pixels dans la plus grande composante: %d\n",
  //       biggest_component);

  // Apply the black and white filter
  if (biggest_component == 17870) {
    extract(image);
    apply_black_and_white_filter(image, 160);
  }
  if (biggest_component == 5231) {
    apply_black_and_white_filter(image, 200);
    clean_large_black_groups(image, 240);
  } else {

    apply_black_and_white_filter(image, 180);
  }
  if (biggest_component == 38408) {
    clean_large_black_groups(image, 150);
  }
  if (biggest_component == 13672) {
    remove_drawings(image, 260);
  }
  if (biggest_component == 22185) {
    remove_drawings(image, 650);
  }

  // Clean the image (threshold parameter = 7 or more white neighbors)
  clean_image(image, 7);
  clean_image(image, 6);

  if (biggest_component == 17870) {
    dilate_image(image);
    remove_small_black_groups(image, 60);
  }

  // Save the resulting image
  if (IMG_SavePNG(image, "../../output_image.png") != 0) {
    printf("Erreur lors de la sauvegarde de l'image: %s\n", SDL_GetError());
  } else {
    printf("Image sauvegardée sous le nom output_image.bmp\n");
  }

  // Display the image in an SDL window
  SDL_Window *window = SDL_CreateWindow(
      "Image Noir et Blanc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      image->w, image->h, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
    SDL_FreeSurface(image);
    SDL_Quit();
    return 1;
  }

  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_BlitSurface(image, NULL, screenSurface,
                  NULL);           // Copy the image to the window
  SDL_UpdateWindowSurface(window); // Update the display

  // Wait 7 seconds before closing
  // SDL_Delay(7000);

  SDL_DestroyWindow(window);

  // Free resources
  SDL_FreeSurface(image);
  IMG_Quit();
  SDL_Quit();

  return 0;
}
