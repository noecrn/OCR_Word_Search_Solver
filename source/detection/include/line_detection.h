#ifndef LINE_DETECTION_H
#define LINE_DETECTION_H

#include <SDL2/SDL.h>

int detect_columns(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound);
int detect_rows(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound);
void analyze_grid(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound, int* column_count, int* row_count);

#endif