#ifndef WORDS_EXTRACTION_H
#define WORDS_EXTRACTION_H

#include <SDL2/SDL.h>

typedef struct {
	int right_bound;
	int left_bound;
	int top_bound;
	int bottom_bound;
} coordinates;

int count_black_pixels_on_a_line(
	SDL_Surface* surface, 
    int start, 
    int end, 
	int y
);

int count_words (
	SDL_Surface* surface, 
	int list_left, 
	int list_right, 
	int list_top, 
	int list_bottom,
	int white_threshold
);

coordinates* words_extraction (
	SDL_Surface* surface, 
	int list_left, 
	int list_right, 
	int list_top, 
	int list_bottom,
	int white_threshold
);

#endif