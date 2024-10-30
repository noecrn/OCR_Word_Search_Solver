#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // Pour exp()

double sigmoid(double x) { return 1 / (1 + exp(-x)); }

void print_array(double* array, size_t length) {
    printf("[");
    for(size_t i = 0; i < length; i++) {
        printf("%f", array[i]);
        if(i != length - 1) printf(", ");
    }
    printf("]\n");
}

void read_file(char* filename,size_t length,double* array)
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

#define numInputs 2
#define numHiddenNodes 2
#define numOutputs 1

int main(int argc, char *argv[])
{
    double hiddenLayer[numHiddenNodes];
    double outputLayer[numOutputs];
    
    double hiddenLayerBias[numHiddenNodes];
    double outputLayerBias[numOutputs];

    double hiddenWeights[numInputs][numHiddenNodes];
    double outputWeights[numHiddenNodes][numOutputs];

    read_file("hiddenLayerBias.txt",numHiddenNodes,hiddenLayerBias);
    read_file("outputLayerBias.txt",numOutputs,outputLayerBias);
    read_file_twodim("hiddenWeights.txt",numInputs,numHiddenNodes,hiddenWeights);
    read_file_twodim("outputWeights.txt",numHiddenNodes,numOutputs,outputWeights);

        // Print final weights after training
    fputs ("Final Hidden Weights\n[ ", stdout);
    for (int j=0; j<numHiddenNodes; j++) {
        fputs ("[ ", stdout);
        for(int k=0; k<numInputs; k++) {
            printf ("%f ", hiddenWeights[k][j]);
        }
        fputs ("] ", stdout);
    }
    
    fputs ("]\nFinal Hidden Biases\n[ ", stdout);
    for (int j=0; j<numHiddenNodes; j++) {
        printf ("%f ", hiddenLayerBias[j]);
    }

    fputs ("]\nFinal Output Weights", stdout);
    for (int j=0; j<numOutputs; j++) {
        fputs ("[ ", stdout);
        for (int k=0; k<numHiddenNodes; k++) {
            printf ("%f ", outputWeights[k][j]);
        }
        fputs ("]\n", stdout);
    }

    fputs ("Final Output Biases\n[ ", stdout);
    for (int j=0; j<numOutputs; j++) {
        printf ("%f ", outputLayerBias[j]);
        
    }
    
    fputs ("]\n", stdout);

    double inputs[numInputs] = {atof(argv[1]),atof(argv[2])};

// Forward pass
            
            // Compute hidden layer activation
            for (int j=0; j<numHiddenNodes; j++) {
                double activation = hiddenLayerBias[j];
                 for (int k=0; k<numInputs; k++) {
                    activation += inputs[k] * hiddenWeights[k][j];
                }
                hiddenLayer[j] = sigmoid(activation);
            }
            
            // Compute output layer activation
            for (int j=0; j<numOutputs; j++) {
                double activation = outputLayerBias[j];
                for (int k=0; k<numHiddenNodes; k++) {
                    activation += hiddenLayer[k] * outputWeights[k][j];
                }
                outputLayer[j] = sigmoid(activation);
            }
            
            print_array(outputLayer,numOutputs);
            // Print the results from forward pass
            printf ("Output:%f\n",
                    round(outputLayer[0]));
    
    return 0;

}
