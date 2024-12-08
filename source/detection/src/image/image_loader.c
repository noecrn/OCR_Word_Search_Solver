#include "../../include/image_loader.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int init_sdl_and_img(void) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return 0;
  }

  // Initialize SDL_image
  int flags = IMG_INIT_PNG;
  if ((IMG_Init(flags) & flags) != flags) {
    printf("SDL_image initialization failed: %s\n", IMG_GetError());
    SDL_Quit();
    return 0;
  }

  return 1;
}

void cleanup_sdl_and_img(void) {
  // Quit SDL_image first
  int img_flags = IMG_Init(0);
  if (img_flags > 0) {
    IMG_Quit();
  }

  // Quit SDL
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  SDL_Quit();
}

// Function to load an image
SDL_Surface *load_image(const char *path) {
  // Load the image
  SDL_Surface *loaded_surface = IMG_Load(path);
  if (loaded_surface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path,
           IMG_GetError());
    return NULL;
  }

  // Convert surface to display format
  SDL_Surface *optimized_surface =
      SDL_ConvertSurfaceFormat(loaded_surface, SDL_PIXELFORMAT_RGBA32, 0);
  SDL_FreeSurface(loaded_surface);

  if (optimized_surface == NULL) {
    printf("Unable to optimize image %s! SDL Error: %s\n", path,
           SDL_GetError());
    return NULL;
  }

  return optimized_surface;
}

void cleanup_image_loader() {
  // This function can remain empty as cleanup is now handled by
  // cleanup_sdl_and_img
}