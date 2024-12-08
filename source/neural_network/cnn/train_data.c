#include "init.h"
#include "log.h"
#include "read_init.h"
#include "read_png.h"
#include "train.h"
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

int main() {
  srand(time(NULL));
  clock_t start_train = clock();

  // Définir la configuration du réseau neuronal
  char *init_path = "../init.txt";

  int nbr_layers;
  int nbr_conv_layers;
  int *pixels_per_conv_layer;
  int *neurons_per_layer;
  int *filter_per_conv_layer;
  double learning_rate;
  int nbr_epochs;
  int nbr_logs;
  int nbr_save;
  int nbr_inputs;
  int nbr_outputs;

  read_initialvar(init_path, &nbr_layers, &nbr_conv_layers,
                  &pixels_per_conv_layer, &neurons_per_layer,
                  &filter_per_conv_layer, &learning_rate, &nbr_epochs,
                  &nbr_logs, &nbr_save, &nbr_inputs, &nbr_outputs);

  // verify_read_initialvar(nbr_layers, nbr_conv_layers, pixels_per_conv_layer,
  // neurons_per_layer, filter_per_conv_layer, nbr_inputs, nbr_outputs,
  // learning_rate, nbr_epochs, nbr_logs, nbr_save);

  // Create a neural network with the specified number of inputs, layers,
  // convolutional layers, neurons per layer, and filters per layer
  NeuralNetwork nn = create_neural_network(
      nbr_inputs, nbr_layers, nbr_conv_layers, pixels_per_conv_layer,
      neurons_per_layer, filter_per_conv_layer);

  // debug_neural_network(&nn);

  const char *log_file = "./data/training_log.txt";

  // Nettoyage du fichier de log au début
  FILE *file = fopen(log_file, "w");
  if (file != NULL)
    fclose(file); // Effacer le contenu existant

  int trainingSetOrder[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                            13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
  // int trainingSetOrder[] = {0,1,2,3,4};

  double ***image_save = allocate_image_save(nbr_outputs, nbr_save, nbr_inputs);
  double *train_inputs;
  double *train_outputs = calloc(nbr_outputs, sizeof(double));
  // Boucle d'entraînement

  for (int epoch = 0; epoch < nbr_epochs; epoch++) {

    shuffle(trainingSetOrder, nbr_outputs); // Mélanger les entrées

    for (int i = 0; i < nbr_outputs; i++) {

      int letter = trainingSetOrder[i];

      train_outputs[letter] = 1.0;

      if (epoch < nbr_save) {
        char filename[50];
        int min = 0, max = 15;
        sprintf(filename, "./data/dataset_30x30/%c/%d.png", (char)(letter + 65),
                (rand() % (max - min + 1)) + min);
        train_inputs = image_to_pixel_list(filename);
        image_save[letter][epoch] = train_inputs;
      } else {
        int min = 0, max = nbr_save - 1;
        train_inputs = image_save[letter][(rand() % (max - min + 1)) + min];
      }

      forward_pass(&nn, train_inputs);
      backpropagation(&nn, train_outputs, learning_rate);

      if ((epoch + 1) % (nbr_epochs / nbr_logs) == 0) {
        log_training_attempt(log_file, (char)(letter + 65),
                             nn.layers[nn.nbr_layers - 1].neurons,
                             nn.layers[nn.nbr_layers - 1].nbr_neurons);
      }
      train_outputs[letter] = 0.0;
    }

#define TOLERANCE 1e-6

    if (fmod((((double)(epoch + 1) / (double)nbr_epochs) * 100.0), 10.0) <
        TOLERANCE) {
      int progression = (((double)epoch + 1) / (double)nbr_epochs) * 100.0;
      char result[101];
      memset(result, '-', progression);
      result[progression] = '\0';
      printf("Training : %d%% %s\n", progression, result);
    }
  }
  // Sauvegarde des biais et des poids dans des fichiers texte
  save_neural_network(&nn);
  free_image_save(image_save, nbr_outputs, nbr_save);
  printf("Les poids et biais ont été enregistrés dans des fichiers texte en "
         "%lds.\n",
         (clock() - start_train) / CLOCKS_PER_SEC);

  free_neural_network(&nn);
  return 0;
}