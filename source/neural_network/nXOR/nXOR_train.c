#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Sigmoid activation function
double sigmoid(double x) { return 1 / (1 + exp(-x)); }

// Derivative of the Sigmoid function
double dSigmoid(double x) { return x * (1 - x); }

// Random initialization of weights
double init_weight() { return ((double)rand()) / ((double)RAND_MAX); }

// Shuffle an array
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

// Write a 1D array to a file
void write_file_onedim(char *filename, size_t length, double *array) {
  FILE *fl = fopen(filename, "w");
  if (fl == NULL) {
    printf("Error opening file %s", filename);
    return;
  }

  for (size_t j = 0; j < length; j++) {
    fprintf(fl, "%f\n", array[j]);
  }

  fclose(fl);
}

// Write a 2D array to a file
void write_file_twodim(char *filename, size_t length1, size_t length2,
                       double array[length1][length2]) {
  FILE *fl = fopen(filename, "w");
  if (fl == NULL) {
    printf("Error opening file %s", filename);
    return;
  }

  for (size_t j = 0; j < length1; j++) {
    for (size_t k = 0; k < length2; k++) {
      fprintf(fl, "%f\n", array[j][k]);
    }
  }

  fclose(fl);
}

#define numInputs 2
#define numHiddenNodes 4
#define numOutputs 1
#define numTrainingSets 4

int main(void) {

  const double lr = 0.1f;

  double hiddenLayer[numHiddenNodes];
  double outputLayer[numOutputs];

  double hiddenLayerBias[numHiddenNodes];
  double outputLayerBias[numOutputs];

  double hiddenWeights[numInputs][numHiddenNodes];
  double outputWeights[numHiddenNodes][numOutputs];

  // Training set (inputs and expected outputs)
  double training_inputs[numTrainingSets][numInputs] = {
      {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};
  double training_outputs[numTrainingSets][numOutputs] = {
      {1.0f}, {0.0f}, {0.0f}, {1.0f}};

  // Initialize weights and biases with random values
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

  int trainingSetOrder[] = {0, 1, 2, 3};

  int numberOfEpochs = 10000;

  for (int epochs = 0; epochs < numberOfEpochs; epochs++) {

    shuffle(trainingSetOrder, numTrainingSets);

    for (int x = 0; x < numTrainingSets; x++) {

      int i = trainingSetOrder[x];

      // Forward pass

      // Calculate hidden layer activation
      for (int j = 0; j < numHiddenNodes; j++) {
        double activation = hiddenLayerBias[j];
        for (int k = 0; k < numInputs; k++) {
          activation += training_inputs[i][k] * hiddenWeights[k][j];
        }
        hiddenLayer[j] = sigmoid(activation);
      }

      // Calculate output layer activation
      for (int j = 0; j < numOutputs; j++) {
        double activation = outputLayerBias[j];
        for (int k = 0; k < numHiddenNodes; k++) {
          activation += hiddenLayer[k] * outputWeights[k][j];
        }
        outputLayer[j] = sigmoid(activation);
      }

      // Backpropagation

      // Calculate deltas for output layer weights
      double deltaOutput[numOutputs];
      for (int j = 0; j < numOutputs; j++) {
        double errorOutput = (training_outputs[i][j] - outputLayer[j]);
        deltaOutput[j] = errorOutput * dSigmoid(outputLayer[j]);
      }

      // Calculate deltas for hidden layer weights
      double deltaHidden[numHiddenNodes];
      for (int j = 0; j < numHiddenNodes; j++) {
        double errorHidden = 0.0f;
        for (int k = 0; k < numOutputs; k++) {
          errorHidden += deltaOutput[k] * outputWeights[j][k];
        }
        deltaHidden[j] = errorHidden * dSigmoid(hiddenLayer[j]);
      }

      // Apply changes to output layer weights
      for (int j = 0; j < numOutputs; j++) {
        outputLayerBias[j] += deltaOutput[j] * lr;
        for (int k = 0; k < numHiddenNodes; k++) {
          outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
        }
      }

      // Apply changes to hidden layer weights
      for (int j = 0; j < numHiddenNodes; j++) {
        hiddenLayerBias[j] += deltaHidden[j] * lr;
        for (int k = 0; k < numInputs; k++) {
          hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j] * lr;
        }
      }
    }
  }

  // Save biases and weights to files
  write_file_onedim("data/hiddenLayerBias.txt", numHiddenNodes,
                    hiddenLayerBias);
  write_file_onedim("data/outputLayerBias.txt", numOutputs, outputLayerBias);
  write_file_twodim("data/hiddenWeights.txt", numInputs, numHiddenNodes,
                    hiddenWeights);
  write_file_twodim("data/outputWeights.txt", numHiddenNodes, numOutputs,
                    outputWeights);

  printf("The neural network has been completely trained\n");

  return 0;
}
