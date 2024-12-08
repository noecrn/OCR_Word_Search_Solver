#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition de la taille maximale
#define MAX_ROWS 100
#define MAX_COLS 100

// Directions pour la recherche de mots (8 directions : horizontale, verticale,
// diagonales)
int directions[8][2] = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},
                        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

// Palette de couleurs pour tracer les mots
Uint32 getColor(SDL_Surface *surface, int index) {
  Uint32 colors[] = {
      SDL_MapRGB(surface->format, 255, 0, 0),   // Rouge
      SDL_MapRGB(surface->format, 0, 255, 0),   // Vert
      SDL_MapRGB(surface->format, 0, 0, 255),   // Bleu
      SDL_MapRGB(surface->format, 255, 255, 0), // Jaune
      SDL_MapRGB(surface->format, 0, 255, 255), // Cyan
      SDL_MapRGB(surface->format, 255, 0, 255)  // Magenta
  };
  int palette_size = sizeof(colors) / sizeof(colors[0]);
  return colors[index % palette_size];
}

// Fonction pour tracer une ligne sur une surface SDL
void draw_line(SDL_Surface *surface, int x1, int y1, int x2, int y2,
               Uint32 color, int thickness) {
  int dx = abs(x2 - x1), dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  // Adjust the drawing to account for thickness
  int half_thickness = thickness / 2;

  while (1) {
    // Draw a filled rectangle for thickness
    for (int tx = -half_thickness; tx <= half_thickness; tx++) {
      for (int ty = -half_thickness; ty <= half_thickness; ty++) {
        int nx = x1 + tx;
        int ny = y1 + ty;

        if (nx >= 0 && nx < surface->w && ny >= 0 && ny < surface->h) {
          ((Uint32 *)surface->pixels)[ny * surface->w + nx] = color;
        }
      }
    }

    if (x1 == x2 && y1 == y2)
      break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

// Fonction pour calculer la distance de Levenshtein
int levenshteinDistance(const char *s1, const char *s2) {
  int len1 = strlen(s1), len2 = strlen(s2);
  int matrix[len1 + 1][len2 + 1];

  for (int i = 0; i <= len1; i++) {
    for (int j = 0; j <= len2; j++) {
      if (i == 0)
        matrix[i][j] = j;
      else if (j == 0)
        matrix[i][j] = i;
      else if (s1[i - 1] == s2[j - 1])
        matrix[i][j] = matrix[i - 1][j - 1];
      else
        matrix[i][j] = 1 + fmin(matrix[i - 1][j - 1],
                                fmin(matrix[i - 1][j], matrix[i][j - 1]));
    }
  }
  return matrix[len1][len2];
}

// Fonction pour chercher un mot dans la grille
int findWordInMatrix(char mat[MAX_ROWS][MAX_COLS], int rows, int cols,
                     const char *word, int *startX, int *startY, int *endX,
                     int *endY) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (mat[i][j] == word[0]) {
        for (int d = 0; d < 8; d++) {
          *startX = i;
          *startY = j;
          int x = i, y = j;
          int len = strlen(word);
          int found = 1;
          for (int k = 0; k < len; k++) {
            if (x < 0 || x >= rows || y < 0 || y >= cols ||
                mat[x][y] != word[k]) {
              found = 0;
              break;
            }
            x += directions[d][0];
            y += directions[d][1];
          }
          if (found) {
            *endX = x - directions[d][0];
            *endY = y - directions[d][1];
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

// Fonction pour trouver le mot le plus proche dans la grille
int findClosestWordInMatrix(char mat[MAX_ROWS][MAX_COLS], int rows, int cols,
                            const char *word, int *startX, int *startY,
                            int *endX, int *endY, int *min_distance) {
  char buffer[MAX_COLS + 1];
  *min_distance = INT_MAX;
  int found = 0;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      for (int d = 0; d < 8; d++) {
        int x = i, y = j;
        size_t k = 0;
        while (x >= 0 && x < rows && y >= 0 && y < cols && k < strlen(word)) {
          buffer[k++] = mat[x][y];
          x += directions[d][0];
          y += directions[d][1];
        }
        buffer[k] = '\0';
        int distance = levenshteinDistance(word, buffer);
        if (distance < *min_distance) {
          *min_distance = distance;
          *startX = i;
          *startY = j;
          *endX = x - directions[d][0];
          *endY = y - directions[d][1];
          found = 1;
        }
      }
    }
  }
  return found;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <grille> <liste_de_mots>\n", argv[0]);
    return 1;
  }

  // Charger la grille textuelle
  char mat[MAX_ROWS][MAX_COLS];
  int actual_rows = 0, actual_cols = 0;

  FILE *gridFile = fopen(argv[1], "r");
  if (!gridFile) {
    perror("Erreur lors de l'ouverture du fichier grille");
    return 1;
  }
  while (fgets(mat[actual_rows], MAX_COLS + 1, gridFile)) {
    mat[actual_rows][strcspn(mat[actual_rows], "\n")] = '\0'; // Supprime le \n
    actual_rows++;
  }
  fclose(gridFile);
  actual_cols = strlen(mat[0]); // Toutes les lignes ont la même longueur
  // Charger la liste des mots
  FILE *wordFile = fopen(argv[2], "r");
  if (!wordFile) {
    perror("Erreur lors de l'ouverture du fichier de mots");
    return 1;
  }

  char words[100][50];
  int word_count = 0;
  while (fgets(words[word_count], 50, wordFile)) {
    words[word_count][strcspn(words[word_count], "\n")] =
        '\0'; // Supprime le \n
    word_count++;
  }
  fclose(wordFile);

  // Initialiser SDL et TTF
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Erreur SDL : %s\n", SDL_GetError());
    return 1;
  }
  if (TTF_Init() == -1) {
    printf("Erreur lors de l'initialisation de SDL_ttf : %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

    // Charger une police
    const char *font_path = "../solver/arial_bold.ttf";
    TTF_Font *font = TTF_OpenFont(font_path, 24);
    if (!font) {
        printf("Erreur lors du chargement de la police (%s) : %s\n", font_path, TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Dimensions de la grille
    int cell_width = 40, cell_height = 40;
    int grid_width = actual_cols * cell_width;
    int grid_height = actual_rows * cell_height;

    // Créer une surface blanche pour redessiner la grille
    SDL_Surface *grid_surface = SDL_CreateRGBSurface(0, grid_width, grid_height, 32, 0, 0, 0, 0);
    SDL_FillRect(grid_surface, NULL, SDL_MapRGB(grid_surface->format, 255, 255, 255));

    // Dessiner la grille (lettres)
    SDL_Color text_color = {0, 0, 0, 255};  // Noir
    for (int i = 0; i < actual_rows; i++) {
        for (int j = 0; j < actual_cols; j++) {
            char letter[2] = {mat[i][j], '\0'};
            SDL_Surface *text_surface = TTF_RenderText_Solid(font, letter, text_color);
            if (text_surface) {
                SDL_Rect dest_rect = {j * cell_width, i * cell_height, cell_width, cell_height};
                SDL_BlitSurface(text_surface, NULL, grid_surface, &dest_rect);
                SDL_FreeSurface(text_surface);
            }
        }
    }

    // Chercher et dessiner les mots trouvés
    for (int i = 0; i < word_count; i++) {
        int startX, startY, endX, endY;
        int min_distance;
        Uint32 word_color = getColor(grid_surface, i);

        if (findWordInMatrix(mat, actual_rows, actual_cols, words[i], &startX, &startY, &endX, &endY)) {
      	        draw_line(grid_surface, startY * cell_width + cell_width / 4, startX * cell_height + cell_height / 2, endY * cell_width + cell_width / 4, endX * cell_height + cell_height / 2, word_color,4);
                printf("Mot trouvé : %s (%d,%d) -> (%d,%d)\n", words[i], startX, startY, endX, endY);
        } else if (findClosestWordInMatrix(mat, actual_rows, actual_cols, words[i], &startX, &startY, &endX, &endY, &min_distance)) {
	    draw_line(grid_surface, startY * cell_width + cell_width/4, startX * cell_height + cell_height/2, endY * cell_width + cell_width/4, endX*cell_height + cell_height/2, word_color,2);
            printf("Mot non trouvé : %s. Suggestion proche : (%d,%d) -> (%d,%d), Distance : %d\n", words[i], startX, startY, endX, endY, min_distance);
        } else {
            printf("Mot non trouvé et aucune correspondance proche pour : %s\n", words[i]);
        }
    }

    // Sauvegarder l'image
    if (SDL_SaveBMP(grid_surface, "redrawn_output.bmp") != 0) {
        printf("Erreur lors de la sauvegarde de l'image : %s\n", SDL_GetError());
    } else {
        printf("Image sauvegardée dans 'redrawn_output.bmp'.\n");
    }

    // Nettoyage
    SDL_FreeSurface(grid_surface);
    TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
