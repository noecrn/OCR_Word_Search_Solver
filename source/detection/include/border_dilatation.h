#ifndef BORDER_DILATATION_H
#define BORDER_DILATATION_H

#include <SDL.h>

int is_black_pixel(Uint8 r, Uint8 g, Uint8 b);
int has_black_pixels_on_line(SDL_Surface* surface, int y, int left, int right);
int has_black_pixels_on_column(SDL_Surface* surface, int x, int top, int bottom);
void check_and_dilate_borders(SDL_Surface* surface, int* left, int* right, int* top, int* bottom);

#endif