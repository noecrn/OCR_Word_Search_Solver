#include "read_init.h"
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

void read_initialvar(char *init_path, int *nbr_layers, int *nbr_conv_layers,
                     int **pixels_per_conv_layer, int **neurons_per_layer,
                     int **filter_per_conv_layer, double *learning_rate,
                     int *nbr_epochs, int *nbr_logs, int *nbr_save,
                     int *nbr_inputs, int *nbr_outputs) {
  FILE *file = fopen(init_path, "r");
  if (file == NULL) {
    perror("Erreur lors de l'ouverture du fichier");
    exit(EXIT_FAILURE);
  }

  char myString[100];

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre de couches
  if (fgets(myString, sizeof(myString), file)) {
    sscanf(myString, "%d", nbr_layers);
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre de convolutions
  if (fgets(myString, sizeof(myString), file)) {
    sscanf(myString, "%d", nbr_conv_layers);
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre de neurones par layer
  if (fgets(myString, sizeof(myString), file)) {
    *pixels_per_conv_layer = (int *)malloc((*nbr_conv_layers) * sizeof(int));
    char *token = strtok(myString, " ");
    for (int i = 0; i < *nbr_conv_layers; i++) {
      if (token == NULL) {
        fprintf(stderr,
                "Erreur : moins de neurones spécifiés que de couches\n");
        free(*pixels_per_conv_layer);
        fclose(file);
        exit(EXIT_FAILURE);
      }
      (*pixels_per_conv_layer)[i] =
          atoi(token); // Conversion correcte en entier
      token = strtok(NULL, " ");
    }
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre de neurones par layer
  if (fgets(myString, sizeof(myString), file)) {
    *neurons_per_layer = (int *)malloc((*nbr_layers) * sizeof(int));
    char *token = strtok(myString, " ");
    for (int i = 0; i < *nbr_layers; i++) {
      if (token == NULL) {
        fprintf(stderr,
                "Erreur : moins de neurones spécifiés que de couches\n");
        free(*neurons_per_layer);
        fclose(file);
        exit(EXIT_FAILURE);
      }
      (*neurons_per_layer)[i] = atoi(token); // Conversion correcte en entier
      token = strtok(NULL, " ");
    }
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre de filtre par layer
  if (fgets(myString, sizeof(myString), file)) {
    *filter_per_conv_layer = (int *)malloc((*nbr_conv_layers) * sizeof(int));
    char *token = strtok(myString, " ");
    for (int i = 0; i < *nbr_conv_layers; i++) {
      if (token == NULL) {
        fprintf(stderr,
                "Erreur : moins de neurones spécifiés que de couches\n");
        free(*filter_per_conv_layer);
        fclose(file);
      }
      (*filter_per_conv_layer)[i] = atoi(token);
      token = strtok(NULL, " ");
    }
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le taux d'apprentissage
  if (fgets(myString, sizeof(myString), file)) {
    sscanf(myString, "%lf", learning_rate);
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre d'époque
  if (fgets(myString, sizeof(myString), file)) {
    sscanf(myString, "%d", nbr_epochs);
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre de log
  if (fgets(myString, sizeof(myString), file)) {
    sscanf(myString, "%d", nbr_logs);
  } else goto error;

  if (fgets(myString, sizeof(myString), file) == NULL) goto error;

  // Lire le nombre de log
  if (fgets(myString, sizeof(myString), file)) {
    sscanf(myString, "%d", nbr_save);
  } else goto error;

  *nbr_inputs = (*pixels_per_conv_layer)[0];
  *nbr_outputs = (*neurons_per_layer)[*nbr_layers - 1];

  fclose(file);
  return;

error:
  fclose(file);
  fprintf(stderr, "Error reading from file\n");
  exit(EXIT_FAILURE);
}