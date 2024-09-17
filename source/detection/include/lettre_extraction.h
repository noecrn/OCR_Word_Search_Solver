#ifndef LETTRE_EXTRACTION_H
#define LETTRE_EXTRACTION_H

#include <SDL2/SDL.h>

void split_grid_into_images(SDL_Surface* image, int left, int right, int top, int bottom, int num_rows, int num_cols);

#endif