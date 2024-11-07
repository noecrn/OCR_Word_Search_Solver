#include "../include/grid_detection.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    exit(1);
  }

  const char *filename = argv[1];
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    errx(EXIT_FAILURE, "Failed to initialize SDL: %s", SDL_GetError());
  }

  SDL_Surface *image = IMG_Load(filename);
  if (!image) {
    errx(EXIT_FAILURE, "Failed to load image: %s", SDL_GetError());
  }

  int count = detect_grid(image);
  printf("%d\n", count);

  SDL_FreeSurface(image);
  SDL_Quit();
  return 0;
}
