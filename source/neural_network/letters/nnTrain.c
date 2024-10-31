#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // Pour exp()

double sigmoid(double x) { return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return x * (1 - x); }
double init_weight() { return ((double)rand() / (double)RAND_MAX) * 2 - 1; } // Poids centrés autour de 0

void load_training_inputs(char* letter, double* training) {
    SDL_Surface* surface = IMG_Load(letter);
    if (surface == NULL) {
        printf("Erreur lors du chargement de l'image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }
    Uint8* pixels = (Uint8*)surface->pixels;
    int pitch = surface->pitch;
    size_t i = 0;
    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            Uint32 pixel_offset = y * pitch + x * surface->format->BytesPerPixel;
            Uint32 pixel;
            memcpy(&pixel, &pixels[pixel_offset], surface->format->BytesPerPixel);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            double grayscale = 0.299 * r + 0.587 * g + 0.114 * b;
            training[i] = grayscale / 255.0;
            i++;
        }
    }
    SDL_FreeSurface(surface);
}

void print_array(double* array, size_t length) {
    printf("[");
    for(size_t i = 0; i < length; i++) {
        printf("%f", array[i]);
        if(i != length - 1) printf(", ");
    }
    printf("]\n");
}

void shuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

#define numInputs 49
#define numHiddenNodes 49
#define numOutputs 26
#define numTrainingSets 5

int main (void) {
    const double lr = 0.1;
    double hiddenLayer[numHiddenNodes];
    double outputLayer[numOutputs];
    double hiddenLayerBias[numHiddenNodes];
    double outputLayerBias[numOutputs];
    double hiddenWeights[numInputs][numHiddenNodes];
    double outputWeights[numHiddenNodes][numOutputs];

    char* pictures_path[] = {"letters/A.png", "letters/B.png", "letters/C.png", "letters/D.png", "letters/E.png"};
    double training_inputs[numTrainingSets][numInputs];
    double training_outputs[numTrainingSets][numOutputs];

    for(size_t i = 0; i < numTrainingSets; i++) {
        load_training_inputs(pictures_path[i], training_inputs[i]);
        for(size_t j = 0; j < numOutputs; j++) {
            training_outputs[i][j] = (j == i) ? 1.0 : 0.0;
        }
    }

    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numHiddenNodes; j++) {
            hiddenWeights[i][j] = init_weight();
        }
    }
    for (int i = 0; i < numHiddenNodes; i++) {
        hiddenLayerBias[i] = init_weight();
        for (int j = 0; j < numOutputs; j++) {
            outputWeights[i][j] = init_weight();
        }
    }
    for (int i = 0; i < numOutputs; i++) {
        outputLayerBias[i] = init_weight();
    }

    int trainingSetOrder[] = {0, 1, 2, 3, 4};

    int numberOfEpochs = 1000;
    for(int epochs = 0; epochs < numberOfEpochs; epochs++) {
        shuffle(trainingSetOrder, numTrainingSets);

        for (int x = 0; x < numTrainingSets; x++) {
            int i = trainingSetOrder[x];

            // Forward pass
            for (int j = 0; j < numHiddenNodes; j++) {
                double activation = hiddenLayerBias[j];
                for (int k = 0; k < numInputs; k++) {
                    activation += training_inputs[i][k] * hiddenWeights[k][j];
                }
                hiddenLayer[j] = sigmoid(activation);
            }

            for (int j = 0; j < numOutputs; j++) {
                double activation = outputLayerBias[j];
                for (int k = 0; k < numHiddenNodes; k++) {
                    activation += hiddenLayer[k] * outputWeights[k][j];
                }
                outputLayer[j] = sigmoid(activation);
            }

            // Prédiction : trouver l'indice avec la valeur la plus élevée dans outputLayer
            int predicted_label = 0;
            double max_output = outputLayer[0];
            for (int j = 1; j < numOutputs; j++) {
                if (outputLayer[j] > max_output) {
                    max_output = outputLayer[j];
                    predicted_label = j;
                }
            }
            // Vérifier si la prédiction est correcte
            printf("Prédiction : %d, Label attendu : %d\n", predicted_label, i);

            // Backpropagation
            double deltaOutput[numOutputs];
            for (int j = 0; j < numOutputs; j++) {
                double errorOutput = training_outputs[i][j] - outputLayer[j];
                deltaOutput[j] = errorOutput * dSigmoid(outputLayer[j]);
            }

            double deltaHidden[numHiddenNodes];
            for (int j = 0; j < numHiddenNodes; j++) {
                double errorHidden = 0.0;
                for(int k = 0; k < numOutputs; k++) {
                    errorHidden += deltaOutput[k] * outputWeights[j][k];
                }
                deltaHidden[j] = errorHidden * dSigmoid(hiddenLayer[j]);
            }

            for (int j = 0; j < numOutputs; j++) {
                outputLayerBias[j] += deltaOutput[j] * lr;
                for (int k = 0; k < numHiddenNodes; k++) {
                    outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
                }
            }

            for (int j = 0; j < numHiddenNodes; j++) {
                hiddenLayerBias[j] += deltaHidden[j] * lr;
                for(int k = 0; k < numInputs; k++) {
                    hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j] * lr;
                }
            }
        }
    }

    char *file1 = "data/hiddenLayerBias.txt";

    // open the file for writing
    FILE *fp1 = fopen(file1, "w");
    if (fp1 == NULL)
    {
        printf("Error opening the file %s", file1);
        return -1;
    }

    for (int j = 0; j < numHiddenNodes; j++) {
        fprintf(fp1, "%f\n",hiddenLayerBias[j]);
    }

    // close the file
    fclose(fp1);

    char *file2 = "data/outputWeights.txt";

    // open the file for writing
    FILE *fp2 = fopen(file2, "w");
    if (fp2 == NULL)
    {
        printf("Error opening the file %s", file2);
        return -1;
    }

    for (int j = 0; j < numOutputs; j++) {
        for (int k = 0; k < numHiddenNodes; k++) {
            fprintf(fp2, "%f\n",outputWeights[k][j]);
        }
    }

    // close the file
    fclose(fp2);

    char *file3 = "data/outputLayerBias.txt";

    // open the file for writing
    FILE *fp3 = fopen(file3, "w");
    if (fp3 == NULL)
    {
        printf("Error opening the file %s", file3);
        return -1;
    }

    for (int j = 0; j < numOutputs; j++) {
        fprintf(fp3, "%f\n",outputLayerBias[j]);
    }

    // close the file
    fclose(fp3);

    char *file4 = "data/hiddenWeights.txt";

    // open the file for writing
    FILE *fp4 = fopen(file4, "w");
    if (fp4 == NULL)
    {
        printf("Error opening the file %s", file3);
        return -1;
    }

    for (int j = 0; j < numInputs; j++) {
        for (int k = 0; k < numHiddenNodes; k++) {
            fprintf(fp4, "%f\n",hiddenWeights[k][j]);
        }
    }

    // close the file
    fclose(fp4);

    return 0;

}
