#include "read_png.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Définition des structures
typedef struct Filter {
  int side_size;
  double **weight;
  double bias;
} Filter;

typedef struct Image {
  int side_size;
  double **pixels;
  struct Image *previus_image;
  Filter filters;
} Image;

typedef struct Neuron {
  int nbr_weight;
  double *weight;
  double bias;
  double output;
  double error;
} Neuron;

typedef struct Conv_Layer {
  int nbr_images;
  Image *images;
} Conv_Layer;

typedef struct Layer {
  int nbr_neurons;
  Neuron *neurons;
} Layer;

typedef struct NeuralNetwork {
  int nbr_inputs;
  int nbr_layers;
  Layer *layers;
  int nbr_conv_layers;
  Conv_Layer *conv_layers;
} NeuralNetwork;

double *image_to_pixel_list(const char *image_path) {
  // Initialisation de SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
    return NULL;
  }

  // Initialisation de SDL_image
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
    printf("Erreur d'initialisation de SDL_image: %s\n", IMG_GetError());
    SDL_Quit();
    return NULL;
  }

  // Charger l'image
  SDL_Surface *surface = IMG_Load(image_path);
  if (!surface) {
    printf("Erreur de chargement de l'image: %s\n", IMG_GetError());
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  // Conversion de l'image en mode 32 bits (RGBA)
  SDL_Surface *converted_surface =
      SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
  SDL_FreeSurface(surface); // Libérer la surface initiale

  if (!converted_surface) {
    printf("Erreur de conversion de l'image: %s\n", SDL_GetError());
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  int width = converted_surface->w;
  int height = converted_surface->h;
  int pixel_count = width * height; // Nombre total de pixels

  // Allocation de la mémoire pour la liste de pixels
  double *pixels = (double *)malloc(pixel_count * sizeof(double));
  if (!pixels) {
    printf("Erreur d'allocation mémoire\n");
    SDL_FreeSurface(converted_surface);
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  // Parcours de chaque pixel et conversion en noir et blanc (1 = noir, 0 =
  // blanc)
  Uint32 *pixels_data = (Uint32 *)converted_surface->pixels;
  for (int i = 0; i < pixel_count; ++i) {
    Uint32 pixel = pixels_data[i];

    // Extraire les composantes de couleur RGBA
    Uint8 r = (pixel >> 24) & 0xFF;
    Uint8 g = (pixel >> 16) & 0xFF;
    Uint8 b = (pixel >> 8) & 0xFF;

    // Calculer la luminance (moyenne des trois couleurs pour simplifier)
    Uint8 gray = (r + g + b) / 3;

    // Noir si la luminance est inférieure à un seuil, blanc sinon
    pixels[i] = (float)gray / 255.0; // 1 pour noir, 0 pour blanc
  }

  // Libérer la surface convertie
  SDL_FreeSurface(converted_surface);

  // Quitter SDL_image et SDL
  IMG_Quit();
  SDL_Quit();

  return pixels;
}