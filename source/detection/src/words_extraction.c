#include <SDL2/SDL.h>
#include "../include/rendering.h"
#include "../include/bounding_boxes.h"

// Function to count black pixels on a line between two points
int count_black_pixels_on_a_line(
	SDL_Surface* surface, 
    int start, 
    int end, 
	int y,
    int is_draw_line
) {
	if (is_draw_line) {
		draw_line(surface, start, 0, (SDL_Color){255, 0, 0, 255});
	}
	int black_pixel_count = 0;
	for (int x = start; x < end; x++) {
            Uint32 pixel = 0;
            Uint8 r, g, b, a;
            pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
		if (is_black_pixel(r, g, b, 0)) {
			black_pixel_count++;
		}
	}
	return black_pixel_count;
}

// Function to parser the words list
void words_extraction (
	SDL_Surface* surface, 
	int* list_left, 
	int* list_right, 
	int* list_top, 
	int* list_bottom,
	int white_threshold
) {
	// Start from the top and go down to find words of the list
	// draw a line if you pass from black pixels to white pixels and vice versa
	int is_black = 0;

	for (int y = *list_top; y < *list_bottom; y += 1) {
		int black_pixel_count = count_black_pixels_on_a_line(surface, *list_left, *list_right, *list_top, 0);
		if (black_pixel_count > white_threshold) {
			if (!is_black) {
				draw_line(surface, -1, y, (SDL_Color){255, 0, 0, 255});
				is_black = 1;
			}
		} else {
			if (is_black) {
				draw_line(surface, -1, y, (SDL_Color){255, 0, 0, 255});
				is_black = 0;
			}
		}
	}
}