#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 100
#define MAX_COLS 100
#define MAX_WORDS 100

int directions[8][2] = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},
                        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

typedef struct {
  char word[50];
  int startX, startY, endX, endY;
} FoundWord;

Uint32 getColor(SDL_Surface *surface, int index) {
  Uint32 colors[] = {SDL_MapRGB(surface->format, 255, 0, 0),
                     SDL_MapRGB(surface->format, 0, 255, 0),
                     SDL_MapRGB(surface->format, 0, 0, 255),
                     SDL_MapRGB(surface->format, 255, 255, 0),
                     SDL_MapRGB(surface->format, 0, 255, 255),
                     SDL_MapRGB(surface->format, 255, 0, 255)};
  return colors[index % (sizeof(colors) / sizeof(colors[0]))];
}

void draw_line(SDL_Surface *surface, int x1, int y1, int x2, int y2,
               Uint32 color, int thickness) {
  int dx = abs(x2 - x1), dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  int half_thickness = thickness / 2;

  while (1) {
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

void loadFoundWords(const char *filename, FoundWord foundWords[],
                    int *foundCount) {
  FILE *file = fopen(filename, "r");
  if (!file)
    return;

  *foundCount = 0;
  while (fscanf(file, "%s %d %d %d %d", foundWords[*foundCount].word,
                &foundWords[*foundCount].startX,
                &foundWords[*foundCount].startY, &foundWords[*foundCount].endX,
                &foundWords[*foundCount].endY) == 5) {
    (*foundCount)++;
  }

  fclose(file);
}

void saveFoundWords(const char *filename, FoundWord foundWords[],
                    int foundCount) {
  FILE *file = fopen(filename, "w");
  if (!file)
    return;

  for (int i = 0; i < foundCount; i++) {
    fprintf(file, "%s %d %d %d %d\n", foundWords[i].word, foundWords[i].startX,
            foundWords[i].startY, foundWords[i].endX, foundWords[i].endY);
  }

  fclose(file);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <grille> <mot>\n", argv[0]);
    return 1;
  }

  char mat[MAX_ROWS][MAX_COLS];
  int actual_rows = 0, actual_cols = 0;

  FILE *gridFile = fopen(argv[1], "r");
  if (!gridFile) {
    perror("Erreur lors de l'ouverture du fichier grille");
    return 1;
  }
  while (fgets(mat[actual_rows], MAX_COLS + 1, gridFile)) {
    mat[actual_rows][strcspn(mat[actual_rows], "\n")] = '\0';
    actual_rows++;
  }
  fclose(gridFile);
  actual_cols = strlen(mat[0]);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Erreur SDL : %s\n", SDL_GetError());
    return 1;
  }

  const char *word = argv[2];
  FoundWord foundWords[MAX_WORDS];
  int foundCount = 0;

  // Charger les mots déjà trouvés
  loadFoundWords("found_words.txt", foundWords, &foundCount);

  SDL_Surface *grid_surface = SDL_CreateRGBSurface(
      0, actual_cols * 40, actual_rows * 40, 32, 0, 0, 0, 0);
  SDL_FillRect(grid_surface, NULL,
               SDL_MapRGB(grid_surface->format, 255, 255, 255));

  SDL_Color text_color = {0, 0, 0, 255};
  TTF_Init();
  TTF_Font *font = TTF_OpenFont("../solver/arial_bold.ttf", 24);

  for (int i = 0; i < actual_rows; i++) {
    for (int j = 0; j < actual_cols; j++) {
      char letter[2] = {mat[i][j], '\0'};
      SDL_Surface *text_surface =
          TTF_RenderText_Solid(font, letter, text_color);
      SDL_Rect dest_rect = {j * 40, i * 40, 40, 40};
      SDL_BlitSurface(text_surface, NULL, grid_surface, &dest_rect);
      SDL_FreeSurface(text_surface);
    }
  }

  // Dessiner les mots déjà trouvés
  for (int i = 0; i < foundCount; i++) {
    Uint32 color = getColor(grid_surface, i);
    draw_line(grid_surface, foundWords[i].startY * 40 + 10,
              foundWords[i].startX * 40 + 20, foundWords[i].endY * 40 + 10,
              foundWords[i].endX * 40 + 20, color, 4);
  }

  // Trouver le nouveau mot
  int startX, startY, endX, endY;
  if (findWordInMatrix(mat, actual_rows, actual_cols, word, &startX, &startY,
                       &endX, &endY)) {
    Uint32 color = getColor(grid_surface, foundCount);
    draw_line(grid_surface, startY * 40 + 10, startX * 40 + 20, endY * 40 + 10,
              endX * 40 + 20, color, 4);
    printf("Mot trouvé : %s (%d,%d) -> (%d,%d)\n", word, startX, startY, endX,
           endY);

    // Ajouter le mot trouvé à la liste
    strcpy(foundWords[foundCount].word, word);
    foundWords[foundCount].startX = startX;
    foundWords[foundCount].startY = startY;
    foundWords[foundCount].endX = endX;
    foundWords[foundCount].endY = endY;
    foundCount++;
  } else {
    printf("Mot non trouvé : %s\n", word);
  }

  // Sauvegarder les mots trouvés
  saveFoundWords("found_words.txt", foundWords, foundCount);

  SDL_SaveBMP(grid_surface, "updated_grid.bmp");
  SDL_FreeSurface(grid_surface);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
