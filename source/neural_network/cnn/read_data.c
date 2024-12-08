#include "read_data.h"
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

void read_filters_from_file(const char *filename,
                            const Conv_Layer *conv_layer) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Erreur lors de l'ouverture du fichier : %s\n", filename);
    return;
  }

  char myString[50];

  for (int i = 0; i < conv_layer->nbr_images; i++) {
    Image *image = &conv_layer->images[i];
    Filter *filter = &image->filters;
    for (int y = 0; y < filter->side_size; y++) {
      for (int x = 0; x < filter->side_size; x++) {
        if (fgets(myString, sizeof(myString), file)) {
          sscanf(myString, "%lf", &filter->weight[y][x]);
        }
      }
    }
    if (fgets(myString, sizeof(myString), file)) {
      sscanf(myString, "%lf", &filter->bias);
    }
  }
  fclose(file);
}

// Écrire un tableau 1D (biais) dans un fichier
void read_bias_from_file(const char *filename, const Layer *layer) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Erreur lors de l'ouverture du fichier : %s\n", filename);
    return;
  }

  char myString[50];

  for (int i = 0; i < layer->nbr_neurons; i++) {
    if (fgets(myString, sizeof(myString), file)) {
      sscanf(myString, "%lf", &layer->neurons[i].bias);
    }
  }
  fclose(file);
}

// Écrire un tableau 2D (poids) dans un fichier
void read_weights_from_file(const char *filename, const Layer *layer) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Erreur lors de l'ouverture du fichier : %s\n", filename);
    return;
  }

  char myString[50];

  for (int i = 0; i < layer->nbr_neurons; i++) {
    for (int j = 0; j < layer->neurons[i].nbr_weight; j++) {
      if (fgets(myString, sizeof(myString), file)) {
        sscanf(myString, "%lf", &layer->neurons[i].weight[j]);
      }
    }
  }
  fclose(file);
}

// Sauvegarder les poids et les biais d'une couche
void read_layer_from_files(const Layer *layer, int layer_index) {
  char bias_filename[50];
  char weight_filename[50];

  // Créer les noms de fichiers pour les biais et les poids
  sprintf(bias_filename, "../neural_network/cnn/data/layer_%d_biases.txt",
          layer_index);
  sprintf(weight_filename, "../neural_network/cnn/data/layer_%d_weights.txt",
          layer_index);

  // Sauvegarder les biais et les poids
  read_bias_from_file(bias_filename, layer);
  read_weights_from_file(weight_filename, layer);
}

// Sauvegarder les poids et les biais d'une couche
void read_conv_layer_from_files(const Conv_Layer *layer, int layer_index) {
  char filter_filename[50];

  // Créer les noms de fichiers pour les biais et les poids
  sprintf(filter_filename, "../neural_network/cnn/data/filter_%d.txt",
          layer_index);

  read_filters_from_file(filter_filename, layer);
}

// Sauvegarder le réseau complet
void read_data(NeuralNetwork *nn) {
  for (int i = 0; i < nn->nbr_layers; i++) {
    read_layer_from_files(&nn->layers[i], nn->nbr_conv_layers + i);
  }
  for (int i = 0; i < nn->nbr_conv_layers; i++) {
    read_conv_layer_from_files(&nn->conv_layers[i], i);
  }
}
