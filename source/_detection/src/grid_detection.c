#include <SDL2/SDL.h>

#define THRESHOLD 30

// Function to get pixel color
Uint32 get_pixel_color (SDL_Surface *surface, int x, int y) {
	Uint32* pixel = (Uint32*)surface->pixels;
	return pixel[y * surface->w + x];
}

// If the pixel is black, return 1, else return 0
int is_pixel_black (Uint32 pixel, SDL_Surface *surface) {
	Uint8 r, g, b;
	SDL_GetRGB(pixel, surface->format, &r, &g, &b);
	return (r < THRESHOLD && g < THRESHOLD && b < THRESHOLD);
}

// Struct to store pixel coordinates
typedef struct {
	int x;
	int y;
} Pixel;

// Function to check if a pixel is in the grid
int is_pixel_in_grid (int x, int  y, int Height, int Width) {
	return (x >= 0 && x < Width && y >= 0 && y < Height);
}

// Recursive function to propagate and count black pixels
int propagate_and_count (SDL_Surface *surface, int x, int y, int Height, int Width, int* visited) {
	// Check if the pixel is in the grid and has not been visited
	if (!is_pixel_in_grid(x, y, Height, Width) || visited[y * Width + x]) {
		return 0;
	}

	// Get the pixel color
	Uint32 pixel = get_pixel_color(surface, x, y);

	// If the pixel is not black, return 0
	if (!is_pixel_black(pixel, surface)) {
		return 0;
	}

	// Mark the pixel as visited
	visited[y * Width + x] = 1;
	int count = 1;

	// Propagate to the 8 neighbors
	Pixel directions[] = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1, 0},           {1, 0},
        {-1, 1},  {0, 1},  {1, 1}
    };

	// Propagate to the neighbors
	for (int i = 0; i < 8; i++) {
		int new_x = x + directions[i].x;
		int new_y = y + directions[i].y;
		count += propagate_and_count(surface, new_x, new_y, Height, Width, visited);
	}

	return count;
}

// Function to count the number of black pixels
int count_pixels (SDL_Surface *surface, int* visited, int Height, int Width, int start_x, int start_y) {
	// Loop through all the pixels
	for (int y = start_y; y < Height; y++) {
		for (int x = start_x; x < Width; x++) {
			// Get the pixel color
			Uint32 pixel = get_pixel_color(surface, x, y);

			// If the pixel is black and has not been visited, propagate
			if (is_pixel_black(pixel, surface) && !visited[y * Width + x]) {
				int count = propagate_and_count(surface, x, y, Height, Width, visited);
				return count;
			}
		}
	}

	return -1;
}

// Function return the size of the biggest black pixels amount
int detect_grid (SDL_Surface *surface) {
	// Get the image dimensions
	int Height = surface->h;
	int Width = surface->w;

	// Initialize the visited array
	int visited[Height * Width];
	for (int i = 0; i < Height * Width; i++) {
		visited[i] = 0;
	}

	int max_count = 0;

	// Loop through all the pixels
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			// Get the pixel color
			Uint32 pixel = get_pixel_color(surface, x, y);
			
			// If the pixel is black and has not been visited, propagate
			if (is_pixel_black(pixel, surface) && !visited[y * Width + x]) {
				int count = count_pixels(surface, visited, Height, Width, x, y);
				if (count > max_count) {
					max_count = count;
				}
			}
		}
	}

	return max_count;
}
