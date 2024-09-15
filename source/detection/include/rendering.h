#ifndef RENDERING_H
#define RENDERING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void draw_line(SDL_Surface *surface, int x, int y, SDL_Color color);
void draw_grid(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound, int num_rows, int num_cols);
void draw_square(SDL_Surface *surface, int x, int y, int size, SDL_Color color);

#endif