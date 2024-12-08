#include "init.h"
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

// Fonction d'initialisation d'un poids aléatoire avec la méthode de Xavier
double init_weight() { return (double)rand() / (double)RAND_MAX * 2.0 - 1.0; }

// Fonction pour créer un filtre
Filter create_filter(int size) {
  Filter filter;
  filter.side_size = size;

  filter.weight = malloc(size * sizeof(double *));
  if (filter.weight == NULL) {
    perror("Erreur d'allocation des lignes de weight");
    exit(EXIT_FAILURE);
  }

  for (int y = 0; y < size; y++) {
    filter.weight[y] = malloc(size * sizeof(double));
    if (filter.weight[y] == NULL) {
      perror("Erreur d'allocation d'une ligne de weight");
      exit(EXIT_FAILURE);
    }
    for (int x = 0; x < size; x++) {
      filter.weight[y][x] = init_weight();
    }
  }
  filter.bias = init_weight();
  return filter;
}

// Fonction pour créer un neurone
Image create_image(int size, struct Image *previus_image) {
  Image image;

  image.side_size = size;

  image.pixels = malloc(size * sizeof(double *));
  if (image.pixels == NULL) {
    perror("Erreur d'allocation des lignes de pixels");
    exit(EXIT_FAILURE);
  }
  for (int y = 0; y < size; y++) {
    image.pixels[y] = malloc(size * sizeof(double));
    if (image.pixels[y] == NULL) {
      perror("Erreur d'allocation d'une ligne de pixels");
      exit(EXIT_FAILURE);
    }
  }

  image.filters = create_filter(3);

  image.previus_image = previus_image;

  return image;
}

// Fonction pour créer un neurone
Neuron create_neuron(int nbr_weights) {
  Neuron neuron;
  neuron.nbr_weight = nbr_weights;

  neuron.weight = (double *)malloc(nbr_weights * sizeof(double));
  for (int i = 0; i < nbr_weights; i++) {
    neuron.weight[i] = init_weight();
  }

  neuron.bias = init_weight();
  neuron.output = 0.0;
  neuron.error = 0.0;
  return neuron;
}

Conv_Layer create_conv_layer(int nbr_image, int size_image,
                             Conv_Layer *previus_layer) {
  Conv_Layer conv_layer;

  conv_layer.nbr_images = nbr_image;
  conv_layer.images = (Image *)malloc(nbr_image * sizeof(Image));
  for (int i = 0; i < nbr_image; i++) {

    if (previus_layer->images == NULL) {
      Filter empty_filter = {0, NULL, 0.0};
      Image empty_image = {0, NULL, NULL, empty_filter};
      conv_layer.images[i] = create_image(size_image, &empty_image);
    } else {
      Image *previus_image = &previus_layer->images[(
          int)(i * ((float)(previus_layer->nbr_images) / (float)(nbr_image)))];
      conv_layer.images[i] = create_image(size_image, previus_image);
    }
  }

  return conv_layer;
}

// Fonction pour créer une couche
Layer create_layer(int nbr_neurons, int nbr_weights_per_neuron) {
  Layer layer;
  layer.nbr_neurons = nbr_neurons;
  layer.neurons = (Neuron *)malloc(nbr_neurons * sizeof(Neuron));
  for (int i = 0; i < nbr_neurons; i++) {
    layer.neurons[i] = create_neuron(nbr_weights_per_neuron);
  }
  return layer;
}

// Fonction pour créer un réseau neuronal
NeuralNetwork create_neural_network(int nbr_inputs, int nbr_layers,
                                    int nbr_conv_layers,
                                    int *pixels_per_conv_layer,
                                    int *neurons_per_layer,
                                    int *filter_per_layer) {
  srand(time(NULL)); // Initialisation du générateur de nombres aléatoires

  NeuralNetwork nn;

  nn.nbr_inputs = nbr_inputs;

  // Couche full connected
  nn.nbr_layers = nbr_layers;
  nn.layers = (Layer *)malloc(nn.nbr_layers * sizeof(Layer));

  for (int i = 0; i < nn.nbr_layers; i++) {
    int nbr_weights_per_neuron =
        (i == 0) ? nbr_inputs : neurons_per_layer[i - 1];
    nn.layers[i] = create_layer(neurons_per_layer[i], nbr_weights_per_neuron);
  }

  // Couche convolution
  nn.nbr_conv_layers = nbr_conv_layers;
  nn.conv_layers =
      (Conv_Layer *)malloc(nn.nbr_conv_layers * sizeof(Conv_Layer));

  Conv_Layer empty_layer = {0, NULL};
  nn.conv_layers[0] =
      create_conv_layer(1, sqrt(pixels_per_conv_layer[0]), &empty_layer);

  for (int i = 1; i < nn.nbr_conv_layers; i += 2) {

    int nbr_image = filter_per_layer[i];
    Conv_Layer previus_layer = nn.conv_layers[i - 1];
    nn.conv_layers[i] = create_conv_layer(
        nbr_image, sqrt(pixels_per_conv_layer[i] / nbr_image), &previus_layer);
    nn.conv_layers[i + 1] = create_conv_layer(
        nbr_image, sqrt(pixels_per_conv_layer[i + 1] / nbr_image),
        &nn.conv_layers[i]);
  }
  return nn;
}

// Free memory used by the Neural Network
void free_neural_network(NeuralNetwork *nn) {
  for (int l = 0; l < nn->nbr_layers; l++) {
    for (int n = 0; n < nn->layers[l].nbr_neurons; n++) {
      free(nn->layers[l].neurons[n].weight);
    }
    free(nn->layers[l].neurons);
  }
  for (int l = 0; l < nn->nbr_conv_layers; l++) {
    for (int i = 0; i < nn->conv_layers[l].nbr_images; i++) {
      for (int y = 0; y < nn->conv_layers[l].images[i].side_size; y++) {
        free(nn->conv_layers[l].images[i].pixels[y]);
      }
      free(nn->conv_layers[l].images[i].pixels);
    }
    free(nn->conv_layers[l].images);
  }
  free(nn->layers);
  free(nn->conv_layers);
}

double ***allocate_image_save(int letter, int nbr_letter, int pixels) {
  double ***image_save = malloc(letter * sizeof(double **));
  for (int i = 0; i < letter; i++) {
    image_save[i] = malloc(nbr_letter * sizeof(double *));
    for (int j = 0; j < nbr_letter; j++) {
      image_save[i][j] = malloc(pixels * sizeof(double));
    }
  }
  return image_save;
}

void free_image_save(double ***image_save, int letter, int nbr_letter) {
  for (int i = 0; i < letter; i++) {
    for (int j = 0; j < nbr_letter; j++) {
      free(image_save[i][j]);
    }
    free(image_save[i]);
  }
  free(image_save);
}