#include "detection_load_data.h"
#include "cnn.h"
#include "init.h"
#include "log.h"
#include "read_data.h"
#include "read_init.h"
#include "read_png.h"
#include "train.h"
#include "write_data.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Définition des structures
typedef struct Filter {
  int side_size;
  double **weight;
  double bias;
} Filter;

typedef struct Image {
  int side_size;
  double **pixels;
  void *previus_image;
  Filter filters;
} Image;

typedef struct Neuron {
  int nbr_weight;
  double *weight;
  double bias;
  double output;
  double error;
} Neuron;

typedef struct Conv_Layer {
  int nbr_images;
  Image *images;
} Conv_Layer;

typedef struct Layer {
  int nbr_neurons;
  Neuron *neurons;
} Layer;

typedef struct NeuralNetwork {
  int nbr_inputs;
  int nbr_layers;
  Layer *layers;
  int nbr_conv_layers;
  Conv_Layer *conv_layers;
} NeuralNetwork;

double *convertImageToMatrix(const char *imagePath, int *width, int *height) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
    return NULL;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    printf("Erreur d'initialisation de SDL_image : %s\n", IMG_GetError());
    SDL_Quit();
    return NULL;
  }

  SDL_Surface *imageSurface = IMG_Load(imagePath);

  if (!imageSurface) {
    printf("Erreur de chargement de l'image : %s\n", IMG_GetError());
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  // Récupérer la taille de l'image
  *width = imageSurface->w;
  *height = imageSurface->h;

  // Vérification que la surface est valide
  if (imageSurface->pixels == NULL) {
    printf("Erreur : Pixels non accessibles pour l'image %s\n", imagePath);
    SDL_FreeSurface(imageSurface);
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  // Allouer une matrice 1D pour stocker les pixels
  double *pixelMatrix = (double *)malloc((*width) * (*height) * sizeof(double));
  if (!pixelMatrix) {
    printf("Erreur d'allocation mémoire pour la matrice\n");
    SDL_FreeSurface(imageSurface);
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  // Vérification du format de l'image
  SDL_PixelFormat *format = imageSurface->format;
  if (!format) {
    printf("Erreur : Format d'image invalide\n");
    free(pixelMatrix);
    SDL_FreeSurface(imageSurface);
    IMG_Quit();
    SDL_Quit();
    return NULL;
  }

  // Accéder aux pixels et déterminer noir (0) ou blanc (1)
  Uint32 *pixels = (Uint32 *)imageSurface->pixels;

  // Pour chaque pixel, on extrait la composante rouge (R)
  for (int y = 0; y < *height; ++y) {
    for (int x = 0; x < *width; ++x) {
      // Vérification des indices
      if (x >= *width || y >= *height) {
        printf("Erreur : indices hors limites x=%d, y=%d\n", x, y);
        free(pixelMatrix);
        SDL_FreeSurface(imageSurface);
        IMG_Quit();
        SDL_Quit();
        return NULL;
      }

      // Accès au pixel à la position (x, y)
      Uint32 pixel = pixels[y * (*width) + x];

      // Extraction de la composante rouge (R)
      Uint8 r, g, b;
      SDL_GetRGB(pixel, format, &r, &g, &b); // Récupère R, G et B

      // Si le pixel est blanc (255, 255, 255), on l'interprète comme blanc (1),
      // sinon noir (0)
      pixelMatrix[y * (*width) + x] =
          (r == 255 && g == 255 && b == 255) ? 1.0 : 0.0; // Blanc = 1, Noir = 0
    }
  }

  SDL_FreeSurface(imageSurface);
  IMG_Quit();
  SDL_Quit();

  return pixelMatrix;
}

// Fonction pour explorer un dossier et traiter les fichiers
void processImagesFromFolder1(const char *folderPath, const char *outputFile,
                              NeuralNetwork nn) {
  DIR *folder = opendir(folderPath);
  if (!folder) {
    perror("Erreur lors de l'ouverture du dossier");
    return;
  }

  // Variables pour détecter la taille maximale de la grille
  int maxRow = 0, maxCol = 0;

  // Première passe : détection des indices maximum (row, col)
  struct dirent *entry;
  while ((entry = readdir(folder)) != NULL) {
    if (strstr(entry->d_name, "cell_") && strstr(entry->d_name, ".png")) {
      int row, col;
      if (sscanf(entry->d_name, "cell_%d_%d.png", &row, &col) == 2) {
        if (row > maxRow)
          maxRow = row;
        if (col > maxCol)
          maxCol = col;
      }
    }
  }

  // Créer une grille pour contenir les caractères
  maxRow++;
  maxCol++;
  char **textGrid = (char **)malloc(maxRow * sizeof(char *));
  for (int i = 0; i < maxRow; ++i) {
    textGrid[i] = (char *)calloc(maxCol, sizeof(char));
    memset(textGrid[i], ' ', maxCol); // Initialisation à un espace vide
  }

  // Revenir au début du dossier pour le traitement
  rewinddir(folder);

  // Deuxième passe : traitement des fichiers
  while ((entry = readdir(folder)) != NULL) {
    if (strstr(entry->d_name, "cell_") && strstr(entry->d_name, ".png")) {
      int row, col;
      if (sscanf(entry->d_name, "cell_%d_%d.png", &row, &col) == 2) {
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", folderPath,
                 entry->d_name);

        int width, height;

        double *matrix = convertImageToMatrix(filePath, &width, &height);

        if (matrix) {
          char character = detection(nn, matrix);
          textGrid[row][col] = character;
          free(matrix);

        } else {
          printf("Erreur lors du traitement de l'image : %s\n", filePath);
        }
      }
    }
  }

  closedir(folder);

  // Sauvegarder la grille dans un fichier texte
  FILE *output = fopen(outputFile, "w");
  if (!output) {
    perror("Erreur lors de l'ouverture du fichier de sortie");
    return;
  }

  for (int i = 0; i < maxRow; ++i) {
    for (int j = 0; j < maxCol; ++j) {
      fputc(textGrid[i][j], output);
    }
    fputc('\n', output); // Retour à la ligne
  }

  fclose(output);

  // Libérer la mémoire
  for (int i = 0; i < maxRow; ++i) {
    free(textGrid[i]);
  }
  free(textGrid);

  printf("Fichier texte généré : %s\n", outputFile);
}

// Fonction pour explorer un dossier et traiter les fichiers
void processImagesFromFolder2(const char *folderPath, const char *outputFile,
                              NeuralNetwork nn) {
  DIR *folder = opendir(folderPath);
  if (!folder) {
    perror("Erreur lors de l'ouverture du dossier");
    return;
  }

  // Variables pour stocker les mots et leurs lettres
  char **words = NULL;
  int wordCount = 0;

  struct dirent *entry;
  while ((entry = readdir(folder)) != NULL) {
    if (strstr(entry->d_name, "word_") && strstr(entry->d_name, "_letter_") &&
        strstr(entry->d_name, ".png")) {
      int wordIndex, letterIndex;
      // Extraction des indices à partir du nom du fichier
      if (sscanf(entry->d_name, "word_%d_letter_%d.png", &wordIndex,
                 &letterIndex) == 2) {
        // Vérifier si le mot existe déjà dans le tableau de mots, sinon
        // l'ajouter
        if (words == NULL || wordCount <= wordIndex) {
          words = realloc(words, (wordIndex + 1) * sizeof(char *));
          for (int i = wordCount; i <= wordIndex; ++i) {
            words[i] =
                (char *)malloc(256 * sizeof(char)); // Allocation pour un mot
            memset(words[i], 0, 256);               // Initialisation à vide
          }
          wordCount = wordIndex + 1;
        }

        // Créer le chemin complet du fichier image
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", folderPath,
                 entry->d_name);

        int width, height;

        double *matrix = convertImageToMatrix(filePath, &width, &height);

        if (matrix) {
          // Convertir la matrice en lettre et ajouter au mot
          char character = detection(nn, matrix);

          // Trouver la position disponible dans le mot
          int len = strlen(words[wordIndex]);
          if (len < 256 - 1) {
            words[wordIndex][len] = character; // Ajouter la lettre
          } else {
            printf("Mot trop long pour %s\n", entry->d_name);
          }

          free(matrix);
        } else {
          printf("Erreur lors du traitement de l'image : %s\n", filePath);
        }
      }
    }
  }

  closedir(folder);

  // Sauvegarder les mots dans un fichier texte
  FILE *output = fopen(outputFile, "w");
  if (!output) {
    perror("Erreur lors de l'ouverture du fichier de sortie");
    return;
  }

  for (int i = 0; i < wordCount; ++i) {
    fprintf(output, "%s\n", words[i]); // Écrire chaque mot dans le fichier
  }

  fclose(output);

  // Libérer la mémoire
  for (int i = 0; i < wordCount; ++i) {
    free(words[i]);
  }
  free(words);

  printf("Fichier texte généré : %s\n", outputFile);
}

void load_data(char *path) {
  // Définir la configuration du réseau neuronal
  char *init_path = "../neural_network/init.txt";

  int nbr_layers;
  int nbr_conv_layers;
  int *pixels_per_conv_layer;
  int *neurons_per_layer;
  int *filter_per_conv_layer;
  double learning_rate;
  int nbr_epochs;
  int nbr_logs;
  int nbr_save;
  int nbr_inputs;
  int nbr_outputs;

  read_initialvar(init_path, &nbr_layers, &nbr_conv_layers,
                  &pixels_per_conv_layer, &neurons_per_layer,
                  &filter_per_conv_layer, &learning_rate, &nbr_epochs,
                  &nbr_logs, &nbr_save, &nbr_inputs, &nbr_outputs);

  // verify_read_initialvar(nbr_layers, nbr_conv_layers, pixels_per_conv_layer,
  // neurons_per_layer, filter_per_conv_layer, nbr_inputs, nbr_outputs,
  // learning_rate, nbr_epochs, nbr_logs, nbr_save);

  // Create a neural network with the specified number of inputs, layers,
  // convolutional layers, neurons per layer, and filters per layer
  NeuralNetwork nn = create_neural_network(
      nbr_inputs, nbr_layers, nbr_conv_layers, pixels_per_conv_layer,
      neurons_per_layer, filter_per_conv_layer);

  read_data(&nn);

  char folderPath[50];
  const char *outputFile = "output_words.txt";
  sprintf(folderPath, "%s/words", path);

  // Appel de la fonction pour traiter les images du dossier
  printf("Traitement des images dans le dossier : %s\n", folderPath);
  printf("Le résultat sera enregistré dans le fichier : %s\n", outputFile);
  processImagesFromFolder2(folderPath, outputFile, nn);

  outputFile = "output_letter.txt";
  sprintf(folderPath, "%s/cells", path);

  // Appel de la fonction pour traiter les images du dossier
  printf("Traitement des images dans le dossier : %s\n", folderPath);
  printf("Le résultat sera enregistré dans le fichier : %s\n", outputFile);
  processImagesFromFolder1(folderPath, outputFile, nn);

  free_neural_network(&nn);
}
