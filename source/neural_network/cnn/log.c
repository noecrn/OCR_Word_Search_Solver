#include "log.h"
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

int index_max(double *array, size_t lenght) {
  int max_index = 0;
  double max = array[0];
  for (size_t j = 0; j < lenght; j++) {
    if (array[j] > max) {
      max_index = j;
      max = array[j];
    }
  }
  return max_index;
}

int index_max_neurone(Neuron *array, size_t lenght) {
  int max_index = 0;
  Neuron max = array[0];
  for (size_t j = 0; j < lenght; j++) {
    if (array[j].output > max.output) {
      max_index = j;
      max = array[j];
    }
  }
  return max_index;
}

void log_training_attempt(const char *filename, char expected_outputs,
                          Neuron *obtained_outputs, int num_outputs) {
  FILE *file = fopen(filename,
                     "a"); // Mode ajout pour ne pas écraser les logs précédents
  if (file == NULL) {
    printf("Erreur lors de l'ouverture du fichier : %s\n", filename);
    return;
  }

  fprintf(file, "Expected Outputs: %c ", expected_outputs);

  int output = index_max_neurone(obtained_outputs, num_outputs);
  fprintf(file, " | Obtained Outputs: %c | Probability: %f",
          (char)(65 + output), obtained_outputs[output].output);

  fprintf(file, "\n");
  fclose(file);
}