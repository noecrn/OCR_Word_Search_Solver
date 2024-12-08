#include "debug.h"
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
  void *previus_image;
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

void verify_read_initialvar(int nbr_layers, int nbr_conv,
                            int *pixels_per_conv_layer, int *neurons_per_layer,
                            int *filter_per_layer, int nbr_inputs,
                            int nbr_outputs, double learning_rate,
                            int nbr_epochs, int nbr_logs, int nbr_save) {
  printf("=== Vérification des paramètres ===\n");

  // Vérification du nombre de couches
  printf("Nombre de couches (nbr_layers) : %d\n", nbr_layers);
  if (nbr_layers <= 0) {
    printf("Erreur : nbr_layers doit être > 0\n");
  }

  // Vérification du nombre de convolutions
  printf("Nombre de convolutions (nbr_conv) : %d\n", nbr_conv);
  if (nbr_conv <= 0) {
    printf("Erreur : nbr_conv doit être > 0\n");
  }

  // Vérification des pixels par couche de convolution
  printf("Pixels par couche de convolution (pixels_per_conv_layer) : ");
  if (pixels_per_conv_layer != NULL) {
    for (int i = 0; i < nbr_conv; i++) {
      printf("%d ", pixels_per_conv_layer[i]);
      if (pixels_per_conv_layer[i] <= 0) {
        printf("\nErreur : pixels_per_conv_layer[%d] doit être > 0\n", i);
      }
    }
    printf("\n");
  } else {
    printf("Erreur : pixels_per_conv_layer est NULL\n");
  }

  // Vérification des neurones par couche
  printf("Neurones par couche (neurons_per_layer) : ");
  if (neurons_per_layer != NULL) {
    for (int i = 0; i < nbr_layers; i++) {
      printf("%d ", neurons_per_layer[i]);
      if (neurons_per_layer[i] <= 0) {
        printf("\nErreur : neurons_per_layer[%d] doit être > 0\n", i);
      }
    }
    printf("\n");
  } else {
    printf("Erreur : neurons_per_layer est NULL\n");
  }

  // Vérification des filtres par couche
  printf("Filtres par couche (filter_per_layer) : ");
  if (filter_per_layer != NULL) {
    for (int i = 0; i < nbr_conv; i++) {
      printf("%d ", filter_per_layer[i]);
    }
    printf("\n");
  } else {
    printf("Erreur : filter_per_layer est NULL\n");
  }

  // Vérification des entrées et sorties
  printf("Nombre d'entrées (nbr_inputs) : %d\n", nbr_inputs);
  if (nbr_inputs <= 0) {
    printf("Erreur : nbr_inputs doit être > 0\n");
  }

  printf("Nombre de sorties (nbr_outputs) : %d\n", nbr_outputs);
  if (nbr_outputs <= 0) {
    printf("Erreur : nbr_outputs doit être > 0\n");
  }

  // Vérification du taux d'apprentissage
  printf("Taux d'apprentissage (learning_rate) : %.6f\n", learning_rate);
  if (learning_rate <= 0) {
    printf("Erreur : learning_rate doit être > 0\n");
  }

  // Vérification du nombre d'époques
  printf("Nombre d'époques (nbr_epochs) : %d\n", nbr_epochs);
  if (nbr_epochs <= 0) {
    printf("Erreur : nbr_epochs doit être > 0\n");
  }

  // Vérification du nombre de journaux
  printf("Nombre de logs (nbr_logs) : %d\n", nbr_logs);
  if (nbr_logs < 0) {
    printf("Erreur : nbr_logs doit être >= 0\n");
  }

  // Vérification du nombre de sauvegardes
  printf("Nombre de sauvegardes (nbr_save) : %d\n", nbr_save);
  if (nbr_save < 0) {
    printf("Erreur : nbr_save doit être >= 0\n");
  }

  printf("=== Fin de la vérification ===\n");
}

void debug_neural_network(NeuralNetwork *nn) {
  printf("Neural Network Debug Info:\n");
  printf("Number of inputs: %d\n", nn->nbr_inputs);
  printf("Number of layers: %d\n", nn->nbr_layers);
  printf("Number of convolution layers: %d\n", nn->nbr_conv_layers);

  // Afficher les informations des couches full connected
  for (int i = 0; i < nn->nbr_layers; i++) {
    Layer *layer = &nn->layers[i];
    printf("Layer %d:\n", i);
    printf("  Number of neurons: %d\n", layer->nbr_neurons);
  }

  // Afficher les informations des couches de convolution
  for (int i = 0; i < nn->nbr_conv_layers; i++) {
    Conv_Layer *conv_layer = &nn->conv_layers[i];
    printf("Convolution Layer %d:\n", i);
    printf("  Number of images: %d\n", conv_layer->nbr_images);
    for (int j = 0; j < conv_layer->nbr_images; j++) {
      Image *image = &conv_layer->images[j];
      printf("    Image %d:\n", j);
      printf("      Side size: %d\n", image->side_size);
      printf("\n");
      printf("      Filter side size: %d\n", image->filters.side_size);
      printf("      Filter weights: ");
      for (int y = 0; y < image->filters.side_size; y++) {
        for (int x = 0; x < image->filters.side_size; x++) {
          printf("%f ", image->filters.weight[y][x]);
        }
      }
      printf("\n");
      printf("      Filter bias: %f\n", image->filters.bias);
    }
  }
}

void print_array(double array[], int size) {
  for (int i = 0; i < size; i++) {
    printf("%f ", array[i]);
  }
  printf("\n");
}
