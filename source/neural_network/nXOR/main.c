#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction d'activation Sigmoïde
double sigmoid(double x) { return 1 / (1 + exp(-x)); }

// Lecture d'un tableau 1D à partir d'un fichier
void read_file_onedim(char *filename, size_t length, double *array) {
  FILE *fptr;
  fptr = fopen(filename, "r");

  char myString[100];
  size_t i = 0;

  while (fgets(myString, 100, fptr) && i < length) {
    sscanf(myString, "%lf", &array[i]);
    i++;
  }

  fclose(fptr);
}

// Lecture d'un tableau 2D à partir d'un fichier
void read_file_twodim(char *filename, size_t length1, size_t length2,
                      double array[length1][length2]) {
  FILE *fptr;
  fptr = fopen(filename, "r");

  char myString[100];

  for (size_t i = 0; i < length1; i++) {
    for (size_t j = 0; j < length2 && fgets(myString, 100, fptr); j++) {
      sscanf(myString, "%lf", &array[i][j]);
    }
  }

  fclose(fptr);
}

#define numInputs 2
#define numHiddenNodes 4
#define numOutputs 1

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return -1;
  }

  double hiddenLayer[numHiddenNodes];
  double outputLayer[numOutputs];

  double hiddenLayerBias[numHiddenNodes];
  double outputLayerBias[numOutputs];

  double hiddenWeights[numInputs][numHiddenNodes];
  double outputWeights[numHiddenNodes][numOutputs];

  // Lecture des biais et des poids depuis les fichiers
  read_file_onedim("data/hiddenLayerBias.txt", numHiddenNodes, hiddenLayerBias);
  read_file_onedim("data/outputLayerBias.txt", numOutputs, outputLayerBias);
  read_file_twodim("data/hiddenWeights.txt", numInputs, numHiddenNodes,
                   hiddenWeights);
  read_file_twodim("data/outputWeights.txt", numHiddenNodes, numOutputs,
                   outputWeights);

  double inputs[numInputs] = {atof(argv[1]), atof(argv[2])};

  // Calcul de l'activation de la couche cachée
  for (int j = 0; j < numHiddenNodes; j++) {
    double activation = hiddenLayerBias[j];
    for (int k = 0; k < numInputs; k++) {
      activation += inputs[k] * hiddenWeights[k][j];
    }
    hiddenLayer[j] = sigmoid(activation);
  }

  // Calcul de l'activation de la couche de sortie
  for (int j = 0; j < numOutputs; j++) {
    double activation = outputLayerBias[j];
    for (int k = 0; k < numHiddenNodes; k++) {
      activation += hiddenLayer[k] * outputWeights[k][j];
    }
    outputLayer[j] = sigmoid(activation);
  }

  // Affichage de la sortie
  printf("Output: %f\n", round(outputLayer[0]));

  return 1;
}
