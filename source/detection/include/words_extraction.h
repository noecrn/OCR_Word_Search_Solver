#ifndef WORDS_EXTRACTION_H
#define WORDS_EXTRACTION_H

#include <SDL2/SDL.h>

typedef struct {
	int top_bound;
	int bottom_bound;
} coordinates;

int count_black_pixels_on_a_line(
	SDL_Surface* surface, 
    int start, 
    int end, 
	int y, 
	int x
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
	int white_threshold, 
	int word_count
);

void letters_extraction (
	SDL_Surface* surface, 
	int list_left, 
	int list_right, 
	int list_top, 
	int list_bottom,
	int white_threshold, 
	int current_word
);

#endif