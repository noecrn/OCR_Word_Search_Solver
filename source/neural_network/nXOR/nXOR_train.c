#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Fonction d'activation Sigmoïde
double sigmoid(double x) { return 1 / (1 + exp(-x)); }

// Dérivée de la fonction Sigmoïde
double dSigmoid(double x) { return x * (1 - x); }

// Initialisation aléatoire des poids
double init_weight() { return ((double)rand())/((double)RAND_MAX); }

// Mélanger un tableau
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

// Écrire un tableau 1D dans un fichier
void write_file_onedim(char* filename, size_t length, double* array)
{
    FILE *fl = fopen(filename, "w");
    if (fl == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s", filename);
        return;
    }

    for (size_t j = 0; j < length; j++) 
    {
        fprintf(fl, "%f\n", array[j]);
    }

    fclose(fl);
}

// Écrire un tableau 2D dans un fichier
void write_file_twodim(char* filename, size_t length1, size_t length2, double array[length1][length2])
{
    FILE *fl = fopen(filename, "w");
    if (fl == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier %s", filename);
        return;
    }

    for (size_t j = 0; j < length1; j++) 
    {
        for (size_t k = 0; k < length2; k++) 
        {
            fprintf(fl, "%f\n", array[j][k]);
        }
    }

    fclose(fl);
}

#define numInputs 2
#define numHiddenNodes 4
#define numOutputs 1
#define numTrainingSets 4

int main (void) {

    const double lr = 0.1f;
    
    double hiddenLayer[numHiddenNodes];
    double outputLayer[numOutputs];
    
    double hiddenLayerBias[numHiddenNodes];
    double outputLayerBias[numOutputs];

    double hiddenWeights[numInputs][numHiddenNodes];
    double outputWeights[numHiddenNodes][numOutputs];
    
    // Ensemble d'entraînement (entrées et sorties attendues)
    double training_inputs[numTrainingSets][numInputs] = {{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f}};
    double training_outputs[numTrainingSets][numOutputs] = {{1.0f},{0.0f},{0.0f},{1.0f}};
    
    // Initialisation des poids et des biais avec des valeurs aléatoires
    for (int i = 0; i < numInputs; i++) 
    {
        for (int j = 0; j < numHiddenNodes; j++) 
        {
            hiddenWeights[i][j] = init_weight();
        }
    }
    for (int i = 0; i < numHiddenNodes; i++) 
    {
        hiddenLayerBias[i] = init_weight();
        for (int j = 0; j < numOutputs; j++) 
        {
            outputWeights[i][j] = init_weight();
        }
    }
    for (int i = 0; i < numOutputs; i++) {
        outputLayerBias[i] = init_weight();
    }
    
    int trainingSetOrder[] = {0,1,2,3};
    
    int numberOfEpochs = 10000;

    for(int epochs = 0; epochs < numberOfEpochs; epochs++) 
    {

        shuffle(trainingSetOrder, numTrainingSets);

        for (int x = 0; x < numTrainingSets; x++) 
        {
            
            int i = trainingSetOrder[x];
            
            // Passage avant (Forward pass)
            
            // Calcul de l'activation de la couche cachée
            for (int j = 0; j < numHiddenNodes; j++) 
            {
                double activation = hiddenLayerBias[j];
                for (int k = 0; k < numInputs; k++) 
                {
                    activation += training_inputs[i][k] * hiddenWeights[k][j];
                }
                hiddenLayer[j] = sigmoid(activation);
            }
            
            // Calcul de l'activation de la couche de sortie
            for (int j = 0; j < numOutputs; j++) 
            {
                double activation = outputLayerBias[j];
                for (int k = 0; k < numHiddenNodes; k++) 
                {
                    activation += hiddenLayer[k] * outputWeights[k][j];
                }
                outputLayer[j] = sigmoid(activation);
            }

            // Rétropropagation (Backpropagation)
            
            // Calcul des deltas pour les poids de la couche de sortie
            double deltaOutput[numOutputs];
            for (int j = 0; j < numOutputs; j++) 
            {
                double errorOutput = (training_outputs[i][j] - outputLayer[j]);
                deltaOutput[j] = errorOutput * dSigmoid(outputLayer[j]);
            }
            
            // Calcul des deltas pour les poids de la couche cachée
            double deltaHidden[numHiddenNodes];
            for (int j = 0; j < numHiddenNodes; j++) 
            {
                double errorHidden = 0.0f;
                for(int k = 0; k < numOutputs; k++) 
                {
                    errorHidden += deltaOutput[k] * outputWeights[j][k];
                }
                deltaHidden[j] = errorHidden * dSigmoid(hiddenLayer[j]);
            }
            
            // Application des changements aux poids de la couche de sortie
            for (int j = 0; j < numOutputs; j++) 
            {
                outputLayerBias[j] += deltaOutput[j] * lr;
                for (int k = 0; k < numHiddenNodes; k++) 
                {
                    outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * lr;
                }
            }
            
            // Application des changements aux poids de la couche cachée
            for (int j = 0; j < numHiddenNodes; j++) 
            {
                hiddenLayerBias[j] += deltaHidden[j] * lr;
                for(int k = 0; k < numInputs; k++) 
                {
                    hiddenWeights[k][j] += training_inputs[i][k] * deltaHidden[j] * lr;
                }
            }
        }
    }

    // Sauvegarde des biais et poids dans les fichiers
    write_file_onedim("data/hiddenLayerBias.txt", numHiddenNodes, hiddenLayerBias);
    write_file_onedim("data/outputLayerBias.txt", numOutputs, outputLayerBias);
    write_file_twodim("data/hiddenWeights.txt", numInputs, numHiddenNodes, hiddenWeights);
    write_file_twodim("data/outputWeights.txt", numHiddenNodes, numOutputs, outputWeights);

    return 0;
}
