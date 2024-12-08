#include "../../include/bounding_boxes.h"
#include "../../include/lettre_extraction.h"
#include "../../include/rendering.h"
#include <SDL2/SDL.h>
#include <stdio.h>

typedef struct {
  int top_bound;
  int bottom_bound;
} coordinates;

// Function to count black pixels on a line between two points
int count_black_pixels_on_a_line(SDL_Surface *surface, int start, int end,
                                 int y, int x) {
  int black_pixel_count = 0;

  // Ensure we don't exceed surface boundaries
  if (x == -1) { // Horizontal line
    // Clamp values to surface boundaries
    start = (start < 0) ? 0 : start;
    end = (end >= surface->w) ? surface->w - 1 : end;

    if (y < 0 || y >= surface->h)
      return 0;

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / 4;

    for (int i = start; i <= end; i++) {
      Uint32 pixel = pixels[y * pitch + i];
      Uint8 r, g, b, a;
      SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
      if (is_black_pixel(r, g, b, 1)) {
        black_pixel_count++;
      }
    }
  } else if (y == -1) { // Vertical line
    // Clamp values to surface boundaries
    start = (start < 0) ? 0 : start;
    end = (end >= surface->h) ? surface->h - 1 : end;

    if (x < 0 || x >= surface->w)
      return 0;

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / 4;

    for (int i = start; i <= end; i++) {
      Uint32 pixel = pixels[i * pitch + x];
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
                              int white_threshold, int word_count, int flag) {
  if (word_count <= 0) {
    return NULL;
  }

  coordinates *words_coordinates = malloc(word_count * sizeof(coordinates));
  if (words_coordinates == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
  }

  int is_black = 0;
  int current_word = 0;
  SDL_Color blue = {0, 0, 255, 255};

  for (int y = list_top; y <= list_bottom && current_word < word_count; y++) {
    int black_pixel_count =
        count_black_pixels_on_a_line(surface, list_left, list_right, y, -1);

    if (black_pixel_count > white_threshold) {
      if (!is_black) {
        is_black = 1;
        words_coordinates[current_word].top_bound = y;
        if (flag) {
          draw_horizontal_line(surface, y, list_left, list_right, blue);
        }
      }
    } else {
      if (is_black) {
        is_black = 0;
        words_coordinates[current_word].bottom_bound = y;
        if (flag) {
          draw_horizontal_line(surface, y, list_left, list_right, blue);
        }
        current_word++;
      }
    }
  }

  // Handle case where last word extends to bottom of list
  if (is_black && current_word < word_count) {
    words_coordinates[current_word].bottom_bound = list_bottom;
    current_word++;
  }

  // If we found fewer words than expected, reallocate the array
  if (current_word < word_count) {
    coordinates *temp =
        realloc(words_coordinates, current_word * sizeof(coordinates));
    if (temp) {
      words_coordinates = temp;
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

int lettre_size(SDL_Surface *surface, int word_left, int word_right,
                int word_top, int word_bottom, int white_threshold) {
  int flag = 0;
  int letter_start = -1;

  for (int x = word_left; x <= word_right; x++) {
    int black_pixel_count =
        count_black_pixels_on_a_line(surface, word_top, word_bottom, -1, x);

    if (black_pixel_count > white_threshold) {
      if (!flag) {
        flag = 1;
        letter_start = x;
      }
    } else {
      if (flag) {
        flag = 0;
        // We found a complete letter
        if (letter_start != -1) {
          return x - letter_start; // Return width of first letter found
        }
      }
    }
  }

  // If we get here, no complete letter was found
  return 0;
}

void adjust_word_bound(SDL_Surface *surface, int *word_left, int *word_right,
                       int *word_top, int *word_bottom, int white_threshold) {
  while (count_black_pixels_on_a_line(surface, *word_top, *word_bottom, -1,
                                      *word_left) <= white_threshold) {
    (*word_left)++;
  }

  while (count_black_pixels_on_a_line(surface, *word_top, *word_bottom, -1,
                                      *word_right) <= white_threshold) {
    (*word_right)--;
  }
}

// Function to split the word into letters using the size of a letter and the
// size of a word
int letters_extraction_v2(SDL_Surface *surface, int list_left, int list_right,
                          int list_top, int list_bottom, int white_threshold,
                          int current_word, int letter_size) {
  adjust_word_bound(surface, &list_left, &list_right, &list_top, &list_bottom,
                    white_threshold);

  int word_width = list_right - list_left;
  int current_letter = 0;
  int num_letters = 0;

  if (word_width % letter_size > letter_size / 2) {
    num_letters = word_width / letter_size + 1;
  } else {
    num_letters = word_width / letter_size;
  }

  for (int i = 0; i < num_letters; i++) {
    int letter_left_bound = list_left + i * letter_size;
    int letter_right_bound = list_left + (i + 1) * letter_size;

    if (letter_right_bound > list_right) {
      letter_right_bound = list_right;
    }

    split_word_into_images(surface, letter_left_bound, letter_right_bound,
                           list_top, list_bottom, current_word, current_letter);
    current_letter++;
  }

  return current_letter;
}