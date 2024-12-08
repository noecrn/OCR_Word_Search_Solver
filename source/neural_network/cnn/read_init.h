// Définition des structures
typedef struct Filter Filter;
typedef struct Image Image;
typedef struct Neuron Neuron;
typedef struct Conv_Layer Conv_Layer;
typedef struct Layer Layer;
typedef struct NeuralNetwork NeuralNetwork;

void read_initialvar(char *init_path, int *nbr_layers, int *nbr_conv_layers,
                     int **pixels_per_conv_layer, int **neurons_per_layer,
                     int **filter_per_conv_layer, double *learning_rate,
                     int *nbr_epochs, int *nbr_logs, int *nbr_save,
                     int *nbr_inputs, int *nbr_outputs);