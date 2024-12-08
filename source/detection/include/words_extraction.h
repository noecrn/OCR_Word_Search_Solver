#ifndef WORDS_EXTRACTION_H
#define WORDS_EXTRACTION_H

#include <SDL2/SDL.h>

typedef struct {
  int top_bound;
  int bottom_bound;
} coordinates;

int count_black_pixels_on_a_line(SDL_Surface *surface, int start, int end,
                                 int y, int x);
int count_words(SDL_Surface *surface, int list_left, int list_right,
                int list_top, int list_bottom, int white_threshold);
coordinates *words_extraction(SDL_Surface *surface, int list_left,
                              int list_right, int list_top, int list_bottom,
                              int white_threshold, int word_count, int flag);
int letters_extraction(SDL_Surface *surface, int list_left, int list_right,
                       int list_top, int list_bottom, int white_threshold,
                       int current_word);
int lettre_size(SDL_Surface *surface, int word_left, int word_right,
                int word_top, int word_bottom, int white_threshold);
int letters_extraction_v2(SDL_Surface *surface, int list_left, int list_right,
                          int list_top, int list_bottom, int white_threshold,
                          int current_word, int letter_size);

#endif