#ifndef BOUNRING_BOXES_H
#define BOUNRING_BOXES_H

#include <SDL2/SDL.h>

int is_black_pixel(Uint8 r, Uint8 g, Uint8 b, int black_tolerance);
int count_black_pixels_in_block(SDL_Surface *surface, int start_x, int start_y,
                                int block_size, int black_tolerance,
                                int is_draw_square);
void detect_letter_grid(SDL_Surface *surface, int *left_bound, int *right_bound,
                        int *top_bound, int *bottom_bound, int block_size,
                        int black_tolerance, int white_threshold,
                        int space_threshold);

#endif