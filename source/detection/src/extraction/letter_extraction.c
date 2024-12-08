#include "../../include/grid_detection.h"
#include "../../include/image_processing.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

// Helper function to count black pixels in a surface
int count_black_pixels(SDL_Surface *surface) {
  int black_pixels = 0;
  SDL_LockSurface(surface);

  Uint32 *pixels = (Uint32 *)surface->pixels;
  int total_pixels = surface->w * surface->h;

  for (int i = 0; i < total_pixels; i++) {
    Uint8 r, g, b;
    SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
    // Check if pixel is black (or very dark)
    if (r < 50 && g < 50 && b < 50) {
      black_pixels++;
    }
  }

  SDL_UnlockSurface(surface);
  return black_pixels;
}

// Function to split the base image into smaller images for each grid cell
void split_grid_into_images(SDL_Surface *image, int left, int top, int right,
                            int bottom, int num_rows, int num_cols) {
  float cell_width = (float)(right - left) / num_cols;
  float cell_height = (float)(bottom - top) / num_rows;

  const char *cells_directory = "../data/cells";

  for (int row = 0; row < num_rows; row++) {
    for (int col = 0; col < num_cols; col++) {
      int start_x = left + (int)(col * cell_width);
      int start_y = top + (int)(row * cell_height);

      // Create a surface for the sub-image
      SDL_Rect src_rect = {start_x, start_y, (int)cell_width, (int)cell_height};
      SDL_Surface *cell_surface = SDL_CreateRGBSurfaceWithFormat(
          0, (int)cell_width, (int)cell_height, 30, SDL_PIXELFORMAT_RGBA32);
      SDL_BlitSurface(image, &src_rect, cell_surface, NULL);

      // Generate the filename for the sub-image
      char filename[256];
      snprintf(filename, sizeof(filename), "%s/cell_%d_%d.png", cells_directory,
               row, col);

      // Save the sub-image as a PNG file
      save_image(cell_surface, filename);

      // Free the surface of the sub-image
      SDL_FreeSurface(cell_surface);
    }
  }
}

// Function to extract the letters from the words list
void split_word_into_images(SDL_Surface *surface, int letter_left,
                            int letter_right, int letter_top, int letter_bottom,
                            int current_word, int current_letter) {
  const int FINAL_SIZE = 30;                // Final size for square output
  const float BLACK_PIXEL_THRESHOLD = 0.05; // 5% threshold
  int letter_width = letter_right - letter_left;
  int letter_height = letter_bottom - letter_top;
  const int MIN_SPLIT_WIDTH = 10;    // Minimum width for splitting
  const float SPLIT_THRESHOLD = 1.9; // If width > average * threshold, split

  // Calculate the expected width of a single letter
  int expected_width =
      letter_height * 0.7; // Approximate aspect ratio for letters

  if (letter_width > expected_width * SPLIT_THRESHOLD &&
      letter_width > MIN_SPLIT_WIDTH * 2) {
    // Split the wide letter into parts
    int num_parts = letter_width / expected_width;
    if (num_parts < 2)
      num_parts = 2;
    int part_width = letter_width / num_parts;

    for (int i = 0; i < num_parts; i++) {
      int part_left = letter_left + (i * part_width);
      SDL_Rect src_rect = {part_left, letter_top, part_width, letter_height};

      // Create temporary surface for the letter part
      SDL_Surface *temp_surface = SDL_CreateRGBSurfaceWithFormat(
          0, part_width, letter_height, 30, SDL_PIXELFORMAT_RGBA32);

      if (temp_surface == NULL) {
        fprintf(stderr, "Error creating letter surface: %s\n", SDL_GetError());
        exit(1);
      }

      SDL_BlitSurface(surface, &src_rect, temp_surface, NULL);

      // Create final square surface
      SDL_Surface *final_surface = SDL_CreateRGBSurfaceWithFormat(
          0, FINAL_SIZE, FINAL_SIZE, 30, SDL_PIXELFORMAT_RGBA32);

      // Scale the letter to fit in 30x30 square while maintaining aspect ratio
      SDL_Rect dest_rect;
      float scale = fmin((float)FINAL_SIZE / part_width,
                         (float)FINAL_SIZE / letter_height);
      dest_rect.w = (int)(part_width * scale);
      dest_rect.h = (int)(letter_height * scale);
      dest_rect.x = (FINAL_SIZE - dest_rect.w) / 2;
      dest_rect.y = (FINAL_SIZE - dest_rect.h) / 2;

      // Fill final surface with white background
      SDL_FillRect(final_surface, NULL,
                   SDL_MapRGBA(final_surface->format, 255, 255, 255, 255));

      // Scale and copy the letter to the center of the square
      SDL_BlitScaled(temp_surface, NULL, final_surface, &dest_rect);

      int black_pixels = count_black_pixels(final_surface);
      int total_pixels = FINAL_SIZE * FINAL_SIZE;
      if (black_pixels > total_pixels * BLACK_PIXEL_THRESHOLD) {
        char filename[256];
        current_letter += i;
        snprintf(filename, sizeof(filename), "%s/word_%d_letter_%d.png",
                 "../data/words", current_word, current_letter);
        save_image(final_surface, filename);
      }
      SDL_FreeSurface(temp_surface);
      SDL_FreeSurface(final_surface);
    }
  } else {
    // Handle normal-width letters
    SDL_Rect src_rect = {letter_left, letter_top, letter_width, letter_height};
    SDL_Surface *temp_surface = SDL_CreateRGBSurfaceWithFormat(
        0, letter_width, letter_height, 30, SDL_PIXELFORMAT_RGBA32);

    if (temp_surface == NULL) {
      fprintf(stderr, "Error creating letter surface: %s\n", SDL_GetError());
      exit(1);
    }

    SDL_BlitSurface(surface, &src_rect, temp_surface, NULL);

    // Create final square surface
    SDL_Surface *final_surface = SDL_CreateRGBSurfaceWithFormat(
        0, FINAL_SIZE, FINAL_SIZE, 30, SDL_PIXELFORMAT_RGBA32);

    // Scale the letter to fit in 30x30 square while maintaining aspect ratio
    SDL_Rect dest_rect;
    float scale = fmin((float)FINAL_SIZE / letter_width,
                       (float)FINAL_SIZE / letter_height);
    dest_rect.w = (int)(letter_width * scale);
    dest_rect.h = (int)(letter_height * scale);
    dest_rect.x = (FINAL_SIZE - dest_rect.w) / 2;
    dest_rect.y = (FINAL_SIZE - dest_rect.h) / 2;

    // Fill final surface with white background
    SDL_FillRect(final_surface, NULL,
                 SDL_MapRGBA(final_surface->format, 255, 255, 255, 255));

    // Scale and copy the letter to the center of the square
    SDL_BlitScaled(temp_surface, NULL, final_surface, &dest_rect);

    int black_pixels = count_black_pixels(final_surface);
    int total_pixels = FINAL_SIZE * FINAL_SIZE;
    if (black_pixels > total_pixels * BLACK_PIXEL_THRESHOLD) {
      char filename[256];
      snprintf(filename, sizeof(filename), "%s/word_%d_letter_%d.png",
               "../data/words", current_word, current_letter);
      save_image(final_surface, filename);
    }
    SDL_FreeSurface(temp_surface);
    SDL_FreeSurface(final_surface);
  }
}
