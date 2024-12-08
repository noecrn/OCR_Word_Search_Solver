#include <stdio.h>

// Définition des structures
typedef struct Filter Filter;
typedef struct Image Image;
typedef struct Neuron Neuron;
typedef struct Conv_Layer Conv_Layer;
typedef struct Layer Layer;
typedef struct NeuralNetwork NeuralNetwork;

void write_filters_to_file(const Filter *filter, FILE *file);

void write_biases_to_file(const char *filename, const Layer *layer);

void write_weights_to_file(const char *filename, const Layer *layer);

void save_conv_layer_to_files(const Conv_Layer *layer, int layer_index);

void save_neural_network(NeuralNetwork *nn);