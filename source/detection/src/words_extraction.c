#include "../include/bounding_boxes.h"
#include "../include/lettre_extraction.h"
#include "../include/rendering.h"
#include <SDL2/SDL.h>

typedef struct {
  int top_bound;
  int bottom_bound;
} coordinates;

// Function to count black pixels on a line between two points
int count_black_pixels_on_a_line(SDL_Surface *surface, int start, int end,
                                 int y, int x) {
  int black_pixel_count = 0;

  if (x == -1) {
    for (int i = start; i <= end; i++) {
      Uint32 pixel = ((Uint32 *)surface->pixels)[y * (surface->pitch / 4) + i];
      Uint8 r, g, b, a;
      SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

      if (is_black_pixel(r, g, b, 1)) {
        black_pixel_count++;
      }
    }
  } else if (y == -1) {
    for (int i = start; i <= end; i++) {
      Uint32 pixel = ((Uint32 *)surface->pixels)[i * (surface->pitch / 4) + x];
      Uint8 r, g, b, a;
      SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

      if (is_black_pixel(r, g, b, 1)) {
        black_pixel_count++;
      }
    }
  }

  return black_pixel_count;
}

// Function to count words in the words list
int count_words(SDL_Surface *surface, int list_left, int list_right,
                int list_top, int list_bottom, int white_threshold) {
  int is_black = 0;
  int word_count = 0;

  for (int y = list_top; y <= list_bottom; y++) {
    int black_pixel_count =
        count_black_pixels_on_a_line(surface, list_left, list_right, y, -1);

    if (black_pixel_count > white_threshold) {
      if (!is_black) {
        is_black = 1;
      }
    } else {
      if (is_black) {
        is_black = 0;
        word_count++;
      }
    }
  }

  return word_count;
}

// Function to parser the words list
coordinates *words_extraction(SDL_Surface *surface, int list_left,
                              int list_right, int list_top, int list_bottom,
                              int white_threshold, int word_count) {
  coordinates *words_coordinates = malloc(word_count * sizeof(coordinates));
  if (words_coordinates == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
  }

  int is_black = 0;
  int current_word = 0;

  for (int y = list_top; y <= list_bottom; y++) {
    int black_pixel_count =
        count_black_pixels_on_a_line(surface, list_left, list_right, y, -1);

    if (black_pixel_count > white_threshold) { // draw top line
      if (!is_black) {
        // draw_line(surface, -1, y - 1, (SDL_Color){255, 0, 0, 255});
        is_black = 1;
        words_coordinates[current_word].top_bound = y;
      }
    } else { // draw bottom line
      if (is_black) {
        // draw_line(surface, -1, y, (SDL_Color){255, 0, 0, 255});
        is_black = 0;
        words_coordinates[current_word].bottom_bound = y;
        current_word++;
      }
    }
  }

  return words_coordinates;
}

// Function to parse words
int letters_extraction(SDL_Surface *surface, int list_left, int list_right,
                        int list_top, int list_bottom, int white_threshold,
                        int current_word) {
  int is_black = 0;
  int letter_left_bound = list_left;
  int current_letter = 0;

  for (int x = list_left; x <= list_right; x++) {
    int black_pixel_count =
        count_black_pixels_on_a_line(surface, list_top, list_bottom, -1, x);

    if (black_pixel_count > white_threshold) { // Starting a new letter
      if (!is_black) {
        is_black = 1;
        letter_left_bound = x;
      }
    } else { // Ending a letter
      if (is_black) {
        is_black = 0;
        int letter_right_bound = x;
        split_word_into_images(surface, letter_left_bound, letter_right_bound,
                               list_top, list_bottom, current_word,
                               current_letter);
        current_letter++;
      }
    }
  }

  return current_letter;
}