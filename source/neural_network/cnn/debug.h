// Définition des structures
typedef struct Filter Filter;
typedef struct Image Image;
typedef struct Neuron Neuron;
typedef struct Conv_Layer Conv_Layer;
typedef struct Layer Layer;
typedef struct NeuralNetwork NeuralNetwork;

void verify_read_initialvar(int nbr_layers, int nbr_conv,
                            int *pixels_per_conv_layer, int *neurons_per_layer,
                            int *filter_per_layer, int nbr_inputs,
                            int nbr_outputs, double learning_rate,
                            int nbr_epochs, int nbr_logs, int nbr_save);

void debug_neural_network(NeuralNetwork *nn);

void write_png_grayscale(const char *filename, double **data, int size_image);