// Définition des structures
typedef struct Filter Filter;
typedef struct Image Image;
typedef struct Neuron Neuron;
typedef struct Conv_Layer Conv_Layer;
typedef struct Layer Layer;
typedef struct NeuralNetwork NeuralNetwork;

void read_filters_from_file(const char *filename, const Conv_Layer *conv_layer);

// Écrire un tableau 1D (biais) dans un fichier
void read_bias_from_file(const char *filename, const Layer *layer);

// Écrire un tableau 2D (poids) dans un fichier
void read_weights_from_file(const char *filename, const Layer *layer);

// Sauvegarder les poids et les biais d'une couche
void read_layer_from_files(const Layer *layer, int layer_index);

// Sauvegarder les poids et les biais d'une couche
void read_conv_layer_from_files(const Conv_Layer *layer, int layer_index);

// Sauvegarder le réseau complet
void read_data(NeuralNetwork *nn);