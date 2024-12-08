#include "init.h"
#include "log.h"
#include "read_data.h"
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

char detection(NeuralNetwork nn, double *input) {
  forward_pass(&nn, input);

  int index = index_max_neurone(nn.layers[nn.nbr_layers - 1].neurons,
                                nn.layers[nn.nbr_layers - 1].nbr_neurons);
  return (char)(index + 65);
}

// int main(int argc, char *argv[])
// {
//     // Définir la configuration du réseau neuronal
//     char *init_path = "./data/init.txt";

//     int nbr_layers;
//     int nbr_conv_layers;
//     int *pixels_per_conv_layer;
//     int *neurons_per_layer;
//     int *filter_per_conv_layer;
//     double learning_rate;
//     int nbr_epochs;
//     int nbr_logs;
//     int nbr_save;
//     int nbr_inputs;
//     int nbr_outputs;

//     read_initialvar(init_path,&nbr_layers,&nbr_conv_layers,&pixels_per_conv_layer,
//     &neurons_per_layer,
//     &filter_per_conv_layer,&learning_rate,&nbr_epochs,&nbr_logs,&nbr_save,&nbr_inputs,&nbr_outputs);

//     //verify_read_initialvar(nbr_layers, nbr_conv_layers,
//     pixels_per_conv_layer, neurons_per_layer, filter_per_conv_layer,
//     nbr_inputs, nbr_outputs, learning_rate, nbr_epochs, nbr_logs, nbr_save);

//     // Create a neural network with the specified number of inputs, layers,
//     convolutional layers, neurons per layer, and filters per layer
//     NeuralNetwork nn = create_neural_network(nbr_inputs, nbr_layers,
//     nbr_conv_layers, pixels_per_conv_layer,
//     neurons_per_layer,filter_per_conv_layer);

//     read_data(&nn);

//     //debug_neural_network(&nn);
//     char filename[50];
//     sprintf(filename,
//     "./data/dataset_30x30/%s/%d.png",argv[argc],(int)((double)rand() /
//     (double)RAND_MAX * 149.0 - 0.0)); double *train_inputs =
//     image_to_pixel_list(filename);

//     forward_pass(&nn, train_inputs);

//     int index = index_max_neurone(nn.layers[nn.nbr_layers -
//     1].neurons,nn.layers[nn.nbr_layers - 1].nbr_neurons); printf("Le resultat
//     est : %c, avec une probabilitée de
//     %f%%\n",(char)(index+65),nn.layers[nn.nbr_layers -
//     1].neurons[index].output*100);

//     free_neural_network(&nn);
//     return 0;
// }