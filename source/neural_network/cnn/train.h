#include <stddef.h>

// Définition des structures
typedef struct Filter Filter;
typedef struct Image Image;
typedef struct Neuron Neuron;
typedef struct Conv_Layer Conv_Layer;
typedef struct Layer Layer;
typedef struct NeuralNetwork NeuralNetwork;

double sigmoid(double x);

double sigmoid_derivative(double x);

double relu(double x);

void forward_pass(NeuralNetwork *nn, double *inputs);

void backpropagation(NeuralNetwork *nn, double *expected_outputs,
                     double learning_rate);

void shuffle(int *array, size_t n);

double ***allocate_image_save(int letter, int nbr_letter, int pixels);

void free_image_save(double ***image_save, int letter, int nbr_letter);

void print_array(double array[], int size);