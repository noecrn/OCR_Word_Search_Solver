typedef struct Filter Filter;
typedef struct Image Image;
typedef struct Neuron Neuron;
typedef struct Conv_Layer Conv_Layer;
typedef struct Layer Layer;
typedef struct NeuralNetwork NeuralNetwork;

double init_weight();

Filter create_filter(int size);

Image create_image(int size, struct Image *previus_image);

Neuron create_neuron(int nbr_weights);

Conv_Layer create_conv_layer(int nbr_image, int size_image,
                             Conv_Layer *previus_layer);

Layer create_layer(int nbr_neurons, int nbr_weights_per_neuron);

NeuralNetwork create_neural_network(int nbr_inputs, int nbr_layers,
                                    int nbr_conv_layers,
                                    int *pixels_per_conv_layer,
                                    int *neurons_per_layer,
                                    int *filter_per_layer);

void free_neural_network(NeuralNetwork *nn);

double ***allocate_image_save(int letter, int nbr_letter, int pixels);

void free_image_save(double ***image_save, int letter, int nbr_letter);