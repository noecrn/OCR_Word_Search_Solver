#ifndef LETTRE_EXTRACTION_H
#define LETTRE_EXTRACTION_H

#include <SDL2/SDL.h>

void split_grid_into_images(SDL_Surface *image, int left, int right, int top,
                            int bottom, int num_rows, int num_cols);
void split_word_into_images(SDL_Surface *surface, int letter_left,
                            int letter_right, int letter_top, int letter_bottom,
                            int current_word, int current_letter);

#endif