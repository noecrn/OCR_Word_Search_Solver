#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include <SDL2/SDL.h>

Uint32 get_pixel_color(SDL_Surface *surface, int x, int y);
int is_pixel_black(Uint32 pixel, SDL_Surface *surface);
int is_pixel_in_grid(int x, int y, int Height, int Width);
int propagate_and_count(SDL_Surface *surface, int x, int y, int Height,
                        int Width, int *visited);
int count_pixels(SDL_Surface *surface, int *visited, int Height, int Width,
                 int start_x, int start_y);
int detect_grid(SDL_Surface *surface);

#endif
