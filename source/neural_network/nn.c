#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // Pour exp()

double sigmoid(double x) { return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return x * (1 - x); }

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

void read_file(char* filename,double* array,size_t length)
{
    FILE *fptr;

    // Open a file in read mode
    fptr = fopen(filename, "r");

    // Store the content of the file
    char myString[100];

    // Read the content and print it
    size_t i = 0;
    while(fgets(myString, 100, fptr) && i < length) {
        sscanf(myString, "%lf", &array[i]);
        i++;
    }

    // Close the file
    fclose(fptr);
}

void read_file_twodim(char* filename,size_t length1,size_t length2,double array[length1][length2])
{
    FILE *fptr;

    // Open a file in read mode
    fptr = fopen(filename, "r");

    // Store the content of the file
    char myString[100];

    // Read the content and print it
    for(size_t i = 0;i<length1;i++)
    {
        for(size_t j = 0;j<length2;j++)
        {
            fgets(myString, 100, fptr);
            sscanf(myString, "%lf", &array[i][j]); 
        } 
    }

    // Close the file
    fclose(fptr);
}

#define numInputs 49
#define numHiddenNodes 49
#define numOutputs 26
#define numTrainingSets 5

int main (void) {
    double hiddenLayer[numHiddenNodes];
    double outputLayer[numOutputs];
    double hiddenLayerBias[numHiddenNodes];
    double outputLayerBias[numOutputs];
    double hiddenWeights[numInputs][numHiddenNodes];
    double outputWeights[numHiddenNodes][numOutputs];

    read_file("data/hiddenLayerBias.txt",hiddenLayerBias,numHiddenNodes);
    read_file("data/outputLayerBias.txt",outputLayerBias,numOutputs);
    read_file_twodim("data/hiddenWeights.txt",numInputs,numHiddenNodes,hiddenWeights);
    read_file_twodim("data/outputWeights.txt",numHiddenNodes,numOutputs,outputWeights);

    char* pictures_path[] = {"letters/A.png", "letters/B.png", "letters/C.png", "letters/D.png", "letters/E.png"};
    double inputs[numInputs];

    load_training_inputs(pictures_path[4],inputs);

    // Forward pass
    for (int j = 0; j < numHiddenNodes; j++) {
        double activation = hiddenLayerBias[j];
        for (int k = 0; k < numInputs; k++) {
            activation += inputs[k] * hiddenWeights[k][j];
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
    print_array(outputLayer,26);
    // Vérifier si la prédiction est correcte
    printf("Prédiction : %d", predicted_label);

    
    return 0;

}
