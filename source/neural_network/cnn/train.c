#include "train.h"
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

// Fonction d'activation sigmoid
double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }

// Dérivée de la fonction sigmoid
double sigmoid_derivative(double x) { return x * (1.0 - x); }

double max_pooling(double a, double b, double c, double d) {
  double max = a;
  if (b > max) {
    max = b;
  }
  if (c > max) {
    max = c;
  }
  if (d > max) {
    max = d;
  }
  return max;
}

void print_array_neuron(Neuron *array, int size) {
  for (int i = 0; i < size; i++) {
    printf("%f ", array[i].output);
  }
  printf("\n");
}

// Fonction pour effectuer le forward pass
void forward_pass(NeuralNetwork *nn, double *inputs) {
  Conv_Layer *first_layer = &nn->conv_layers[0];
  Image *first_image = &first_layer->images[0];
  int side_size = first_image->side_size;
  for (int y = 0; y < side_size; y++) {
    for (int x = 0; x < side_size; x++) {
      first_image->pixels[y][x] = inputs[y * side_size + x];
    }
  }

  for (int l = 1; l < nn->nbr_conv_layers; l += 2) {
    Conv_Layer *conv_layer = &nn->conv_layers[l];

    // Convolution
    for (int i = 0; i < conv_layer->nbr_images; i++) {
      Image *image = &conv_layer->images[i];
      int size_side = image->side_size;
      Filter filter = image->filters;
      Image *previus_image = image->previus_image;

      for (int y = 0; y < size_side; y++) {
        for (int x = 0; x < size_side; x++) {
          double sum = 0.0;
          for (int m = 0; m < filter.side_size; m++) {
            for (int n = 0; n < filter.side_size; n++) {
              sum += previus_image->pixels[y + m][x + n] * filter.weight[m][n];
            }
          }
          sum += filter.bias;
          image->pixels[y][x] = sum; // Utiliser ReLU après la convolution
        }
      }
    }

    // Pooling
    Conv_Layer *pooling_layer = &nn->conv_layers[l + 1];
    for (int i = 0; i < pooling_layer->nbr_images; i++) {
      Image *image = &pooling_layer->images[i];
      int size_side = image->side_size;
      Image *previus_image = image->previus_image;

      for (int y = 0; y < size_side; y++) {
        for (int x = 0; x < size_side; x++) {
          image->pixels[y][x] =
              max_pooling(previus_image->pixels[y * 2][x * 2],
                          previus_image->pixels[y * 2][(x * 2) + 1],
                          previus_image->pixels[(y * 2) + 1][x * 2],
                          previus_image->pixels[(y * 2) + 1][(x * 2) + 1]);
        }
      }
    }
  }

  Conv_Layer *last_layer = &nn->conv_layers[nn->nbr_conv_layers - 1];
  Layer *first_fc_layer = &nn->layers[0];
  for (int i = 0; i < last_layer->nbr_images; i++) {
    Image *image = &last_layer->images[i];
    for (int y = 0; y < image->side_size; y++) {
      for (int x = 0; x < image->side_size; x++) {
        first_fc_layer
            ->neurons[i * (image->side_size * image->side_size) +
                      y * image->side_size + x]
            .output = image->pixels[y][x];
      }
    }
  }

  for (int i = 1; i < nn->nbr_layers;
       i++) { // On commence à partir de la 2ème couche
    Layer *layer = &nn->layers[i];
    for (int j = 0; j < layer->nbr_neurons; j++) {
      Neuron *neuron = &layer->neurons[j];
      // Calcul de la somme pondérée (entrées * poids) + biais
      double sum = 0.0;
      for (int k = 0; k < neuron->nbr_weight; k++) {
        sum += nn->layers[i - 1].neurons[k].output * neuron->weight[k];
      }

      sum += neuron->bias;

      // Calcul de la sortie du neurone
      neuron->output = sigmoid(sum);
    }
  }
}

void backpropagation(NeuralNetwork *nn, double *expected_outputs,
                     double learning_rate) {
  // Calcul de l'erreur de sortie
  for (int i = 0; i < nn->layers[nn->nbr_layers - 1].nbr_neurons; i++) {
    Neuron *output_neuron = &nn->layers[nn->nbr_layers - 1].neurons[i];
    // Calcul de l'erreur de chaque neurone de la couche de sortie
    double error = expected_outputs[i] - output_neuron->output;
    output_neuron->error = error * sigmoid_derivative(output_neuron->output);
  }

  // Propagation de l'erreur dans les couches cachées
  for (int i = nn->nbr_layers - 2; i >= 0;
       i--) { // On commence par la couche avant la sortie
    Layer *layer = &nn->layers[i];
    Layer *next_layer = &nn->layers[i + 1];
    for (int j = 0; j < layer->nbr_neurons; j++) {
      Neuron *neuron = &layer->neurons[j];
      double error_sum = 0.0;
      // Somme des erreurs des neurones de la couche suivante, pondérées par
      // leurs poids
      for (int k = 0; k < next_layer->nbr_neurons; k++) {
        error_sum +=
            next_layer->neurons[k].error * next_layer->neurons[k].weight[j];
      }
      // Calcul de l'erreur de chaque neurone de la couche actuelle
      neuron->error = error_sum * sigmoid_derivative(neuron->output);
    }
  }

  // Mise à jour des poids et des biais
  for (int i = 1; i < nn->nbr_layers;
       i++) { // On commence à partir de la deuxième couche
    Layer *layer = &nn->layers[i];
    Layer *previous_layer = &nn->layers[i - 1];
    for (int j = 0; j < layer->nbr_neurons; j++) {
      Neuron *neuron = &layer->neurons[j];
      // Mise à jour des poids et du biais du neurone
      for (int k = 0; k < neuron->nbr_weight; k++) {
        double delta_weight =
            learning_rate * previous_layer->neurons[k].output * neuron->error;
        neuron->weight[k] += delta_weight;
      }
      neuron->bias += learning_rate * neuron->error;
    }
  }
}

void shuffle(int *array, size_t n) {
  if (n > 1) {
    for (size_t i = 0; i < n - 1; i++) {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}