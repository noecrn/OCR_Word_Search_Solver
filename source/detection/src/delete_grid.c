#include "../include/grid_detection.h"
#include <SDL2/SDL.h>

// Function to replace all red pixels with white
void delete_grid(SDL_Surface *surface) {
	int Height = surface->h;
	int Width = surface->w;

	// Loop through each pixel in the image
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			Uint32 pixel = get_pixel_color(surface, x, y);
			if (is_pixel_red(pixel, surface)) {
				// Replace red pixel with white
				Uint32 white = SDL_MapRGB(surface->format, 255, 255, 255);
				put_pixel(surface, x, y, white);
			}
		}
	}
}