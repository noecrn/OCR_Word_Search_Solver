#include <stddef.h>

// Définition des structures
typedef struct Filter Filter;
typedef struct Image Image;
typedef struct Neuron Neuron;
typedef struct Conv_Layer Conv_Layer;
typedef struct Layer Layer;
typedef struct NeuralNetwork NeuralNetwork;

int index_max(double *array, size_t lenght);

int index_max_neurone(Neuron *array, size_t lenght);

void log_training_attempt(const char *filename, char expected_outputs,
                          Neuron *obtained_outputs, int num_outputs);