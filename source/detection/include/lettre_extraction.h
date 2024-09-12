#ifndef LETTRE_EXTRACTION_H
#define LETTRE_EXTRACTION_H

#include <SDL2/SDL.h>

void split_grid_into_images(SDL_Surface* image, int min_x, int max_x, int min_y, int max_y, int num_rows, int num_cols);

#endif