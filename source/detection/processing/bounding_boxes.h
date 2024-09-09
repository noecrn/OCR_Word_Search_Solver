#ifndef BOUNRING_BOXES_H
#define BOUNRING_BOXES_H

#include <SDL2/SDL.h>

void detect_boundaries(SDL_Surface* surface, int* min_x, int* min_y, int* max_x, int* max_y);

#endif