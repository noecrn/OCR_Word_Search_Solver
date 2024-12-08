#include "write_data.h"
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

void write_filters_to_file(const Filter *filter, FILE *file) {
  for (int i = 0; i < filter->side_size; i++) {
    for (int j = 0; j < filter->side_size; j++) {
      fprintf(file, "%f\n", filter->weight[i][j]);
    }
  }
  fprintf(file, "%f\n", filter->bias);
}

// Écrire un tableau 1D (biais) dans un fichier
void write_biases_to_file(const char *filename, const Layer *layer) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Erreur lors de l'ouverture du fichier : %s\n", filename);
    return;
  }
  for (int i = 0; i < layer->nbr_neurons; i++) {
    fprintf(file, "%f\n", layer->neurons[i].bias);
  }
  fclose(file);
}

// Écrire un tableau 2D (poids) dans un fichier
void write_weights_to_file(const char *filename, const Layer *layer) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Erreur lors de l'ouverture du fichier : %s\n", filename);
    return;
  }
  for (int i = 0; i < layer->nbr_neurons; i++) {
    for (int j = 0; j < layer->neurons[i].nbr_weight; j++) {
      fprintf(file, "%f\n", layer->neurons[i].weight[j]);
    }
  }
  fclose(file);
}

// Sauvegarder les poids et les biais d'une couche
void save_layer_to_files(const Layer *layer, int layer_index) {
  char bias_filename[50];
  char weight_filename[50];

  // Créer les noms de fichiers pour les biais et les poids
  sprintf(bias_filename, "./data/layer_%d_biases.txt", layer_index);
  sprintf(weight_filename, "./data/layer_%d_weights.txt", layer_index);

  // Sauvegarder les biais et les poids
  write_biases_to_file(bias_filename, layer);
  write_weights_to_file(weight_filename, layer);
}

// Sauvegarder les poids et les biais d'une couche
void save_conv_layer_to_files(const Conv_Layer *layer, int layer_index) {
  char filter_filename[50];
  // Créer les noms de fichiers pour les biais et les poids
  sprintf(filter_filename, "./data/filter_%d.txt", layer_index);

  FILE *file = fopen(filter_filename, "w");
  for (int i = 0; i < layer->nbr_images; i++) {
    Image *image = &layer->images[i];
    Filter *filter = &image->filters;
    write_filters_to_file(filter, file);
  }
  fclose(file);
}

// Sauvegarder le réseau complet
void save_neural_network(NeuralNetwork *nn) {
  for (int i = 0; i < nn->nbr_layers; i++) {
    save_layer_to_files(&nn->layers[i], nn->nbr_conv_layers + i);
  }
  for (int i = 0; i < nn->nbr_conv_layers; i++) {
    save_conv_layer_to_files(&nn->conv_layers[i], i);
  }
}