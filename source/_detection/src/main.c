#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL.h>

SDL_Surface *load_image (const char *filename) {
  SDL_Surface *image = SDL_LoadBMP(filename);
  if (!image) {
    errx(1, "Unable to load bitmap: %s", SDL_GetError());
  }
  return image;
}

int main (int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    exit(1);
  }

  const char *filename = argv[1];
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    errx(EXIT_FAILURE, "Failed to initialize SDL: %s", SDL_GetError());
  }

  SDL_Surface *image = load_image(filename);
  if (!image) {
    errx(EXIT_FAILURE, "Failed to load image: %s", SDL_GetError());
  }

  // Add your code to process the image here

  SDL_FreeSurface(image);
  SDL_Quit();
  return 0;
}