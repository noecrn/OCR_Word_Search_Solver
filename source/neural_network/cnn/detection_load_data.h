// Définition des structures
typedef struct Filter Filter;

typedef struct Image Image;

typedef struct Neuron Neuron;

typedef struct Conv_Layer Conv_Layer;

typedef struct Layer Layer;

typedef struct NeuralNetwork NeuralNetwork;

double *convertImageToMatrix(const char *imagePath, int *width, int *height);

// Fonction pour explorer un dossier et traiter les fichiers
void processImagesFromFolder1(const char *folderPath, const char *outputFile,
                              NeuralNetwork nn);

// Fonction pour explorer un dossier et traiter les fichiers
void processImagesFromFolder2(const char *folderPath, const char *outputFile,
                              NeuralNetwork nn);

void load_data(char *folderPath);