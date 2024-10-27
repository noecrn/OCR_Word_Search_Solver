#ifndef WORDS_EXTRACTION_H
#define WORDS_EXTRACTION_H

#include <SDL2/SDL.h>

int count_black_pixels_on_a_line(
	SDL_Surface* surface, 
    int start, 
    int end, 
	int y,
    int is_draw_line
);

void words_extraction (
	SDL_Surface* surface, 
	int* list_left, 
	int* list_right, 
	int* list_top, 
	int* list_bottom, 
	int block_size, 
	int black_tolerance, 
	int white_threshold
);

#endif