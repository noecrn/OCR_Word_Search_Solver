#ifndef WORDS_LIST_H
#define WORDS_LIST_H

#include <SDL2/SDL.h>

void adjust_border (
    SDL_Surface* surface,
    int* list_left,
    int* list_right,
    int* list_top,
    int* list_bottom, 
    int block_size, 
    int black_tolerance, 
    int white_threshold,
    int space_threshold
);

void detect_words_list (
    SDL_Surface* surface,
    int* list_left,
    int* list_right,
    int* list_top,
    int* list_bottom,
    int start_x,
    int start_y,
    int block_size, 
    int black_tolerance, 
    int white_threshold,
    int space_threshold,
    int left,
    int right,
    int top,
    int bottom
);

void find_words_list (
    SDL_Surface* surface, 
    int* grid_left,
    int* grid_right,
    int* grid_top,
    int* grid_bottom,
    int* list_left,
    int* list_right,
    int* list_top,
    int* list_bottom,
    int block_size, 
    int black_tolerance, 
    int white_threshold
);

#endif