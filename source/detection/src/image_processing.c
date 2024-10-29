#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Function to rotate an image
void rotate_image(const char *inputPath, const char *outputPath, double angle) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    errx(EXIT_FAILURE, "Failed to initialize SDL: %s", SDL_GetError());
  }

  SDL_Surface *imageSurface = IMG_Load(inputPath);
  if (!imageSurface) {
    errx(EXIT_FAILURE, "Failed to load image: %s", IMG_GetError());
  }

  double angle_rad = angle * M_PI / 180;
  int new_width = ceil(fabs(imageSurface->w * cos(angle_rad)) +
                       fabs(imageSurface->h * sin(angle_rad)));
  int new_height = ceil(fabs(imageSurface->w * sin(angle_rad)) +
                        fabs(imageSurface->h * cos(angle_rad)));

  SDL_Surface *rotatedSurface = SDL_CreateRGBSurface(
      0, new_width, new_height, imageSurface->format->BitsPerPixel,
      imageSurface->format->Rmask, imageSurface->format->Gmask,
      imageSurface->format->Bmask, imageSurface->format->Amask);
  if (!rotatedSurface) {
    SDL_FreeSurface(imageSurface);
    errx(EXIT_FAILURE, "Failed to create rotated surface: %s", SDL_GetError());
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      SDL_CreateWindow("temp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       new_width, new_height, SDL_WINDOW_SHOWN),
      -1, SDL_RENDERER_SOFTWARE);
  if (!renderer) {
    SDL_FreeSurface(imageSurface);
    SDL_FreeSurface(rotatedSurface);
    errx(EXIT_FAILURE, "Failed to create renderer: %s", SDL_GetError());
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
  if (!texture) {
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(imageSurface);
    SDL_FreeSurface(rotatedSurface);
    errx(EXIT_FAILURE, "Failed to create texture: %s", SDL_GetError());
  }

  SDL_Rect dstRect = {(new_width - imageSurface->w) / 2,
                      (new_height - imageSurface->h) / 2, imageSurface->w,
                      imageSurface->h};
  SDL_RenderCopyEx(renderer, texture, NULL, &dstRect, angle, NULL,
                   SDL_FLIP_NONE);
  SDL_RenderPresent(renderer);

  if (IMG_SavePNG(rotatedSurface, outputPath) != 0) {
    errx(EXIT_FAILURE, "Failed to save rotated image: %s", IMG_GetError());
  } else {
    printf("Image saved successfully to: %s\n", outputPath);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_FreeSurface(imageSurface);
  SDL_FreeSurface(rotatedSurface);
  SDL_Quit();
}

// Function to resize an image while preserving aspect ratio
void resize_image(const char *inputPath, const char *outputPath,
                  int minSideSize) {
  // Initialize SDL and SDL_image
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    printf("IMG_Init Error: %s\n", IMG_GetError());
    SDL_Quit();
    return;
  }

  // Load the input image into an SDL_Surface
  SDL_Surface *originalImage = IMG_Load(inputPath);
  if (!originalImage) {
    printf("IMG_Load Error: %s\n", IMG_GetError());
    IMG_Quit();
    SDL_Quit();
    return;
  }

  // Get original image dimensions
  int originalWidth = originalImage->w;
  int originalHeight = originalImage->h;

  // Calculate new dimensions while maintaining aspect ratio
  int newWidth, newHeight;
  if (originalWidth < originalHeight) {
    newWidth = minSideSize;
    newHeight = (minSideSize * originalHeight) / originalWidth;
  } else {
    newHeight = minSideSize;
    newWidth = (minSideSize * originalWidth) / originalHeight;
  }

  // Create a new SDL_Surface with the resized dimensions
  SDL_Surface *resizedImage = SDL_CreateRGBSurface(
      0, newWidth, newHeight, originalImage->format->BitsPerPixel,
      originalImage->format->Rmask, originalImage->format->Gmask,
      originalImage->format->Bmask, originalImage->format->Amask);

  if (!resizedImage) {
    printf("SDL_CreateRGBSurface Error: %s\n", SDL_GetError());
    SDL_FreeSurface(originalImage);
    IMG_Quit();
    SDL_Quit();
    return;
  }

  // Scale the original image to the new surface
  SDL_Rect srcRect = {0, 0, originalWidth, originalHeight};
  SDL_Rect dstRect = {0, 0, newWidth, newHeight};
  if (SDL_BlitScaled(originalImage, &srcRect, resizedImage, &dstRect) != 0) {
    printf("SDL_BlitScaled Error: %s\n", SDL_GetError());
    SDL_FreeSurface(originalImage);
    SDL_FreeSurface(resizedImage);
    IMG_Quit();
    SDL_Quit();
    return;
  }

  // Save the resized image to the output path
  if (IMG_SavePNG(resizedImage, outputPath) != 0) {
    printf("IMG_SavePNG Error: %s\n", IMG_GetError());
  }

  // Free the surfaces
  SDL_FreeSurface(originalImage);
  SDL_FreeSurface(resizedImage);

  // Quit SDL_image and SDL
  IMG_Quit();
  SDL_Quit();
}

// Function to resize an image to a square of a target size
void resize_image_square(const char *inputPath, const char *outputPath,
                         int targetSize) {
  // Load the input image
  SDL_Surface *original_image = IMG_Load(inputPath);
  if (!original_image) {
    fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
    return;
  }

  // Get original image dimensions
  int original_width = original_image->w;
  int original_height = original_image->h;

  // Calculate the scaling factor to fit the image into a targetSize x
  // targetSize square
  float scaling_factor;
  if (original_width > original_height) {
    scaling_factor = (float)targetSize / original_width;
  } else {
    scaling_factor = (float)targetSize / original_height;
  }

  // Calculate the new width and height, preserving the aspect ratio
  int new_width = (int)(original_width * scaling_factor);
  int new_height = (int)(original_height * scaling_factor);

  // Create a new surface for the resized image
  SDL_Surface *resized_image = SDL_CreateRGBSurfaceWithFormat(
      0, targetSize, targetSize, 32, original_image->format->format);
  if (!resized_image) {
    fprintf(stderr, "Error creating resized surface: %s\n", SDL_GetError());
    SDL_FreeSurface(original_image);
    return;
  }

  // Fill the new surface with a white background (RGB: 255, 255, 255)
  SDL_FillRect(resized_image, NULL,
               SDL_MapRGB(resized_image->format, 255, 255, 255));

  // Create a destination rectangle to center the scaled image in the square
  SDL_Rect dest_rect;
  dest_rect.x = (targetSize - new_width) / 2;  // Center horizontally
  dest_rect.y = (targetSize - new_height) / 2; // Center vertically
  dest_rect.w = new_width;
  dest_rect.h = new_height;

  // Scale and blit the original image onto the resized image
  SDL_BlitScaled(original_image, NULL, resized_image, &dest_rect);

  // Save the resized image to the output path
  if (IMG_SavePNG(resized_image, outputPath) != 0) {
    fprintf(stderr, "Error saving image: %s\n", IMG_GetError());
  }

  // Free the surfaces
  SDL_FreeSurface(original_image);
  SDL_FreeSurface(resized_image);
}

// Function to save an SDL_Surface to a PNG file
void save_image(SDL_Surface *surface, const char *outputPath) {
  if (!surface) {
    printf("Invalid surface\n");
    return;
  }

  if (IMG_SavePNG(surface, outputPath) != 0) {
    printf("IMG_SavePNG Error: %s\n", IMG_GetError());
  }
}

// Function to resize cells
void cells_resize(int num_rows, int num_cols) {
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      char cell_filename[50];
      snprintf(cell_filename, sizeof(cell_filename),
               "data/cells/cell_%d_%d.png", i, j);
      resize_image_square(cell_filename, cell_filename, 40);
    }
  }
}