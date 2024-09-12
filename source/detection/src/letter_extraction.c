#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/image_processing.h"

// Function to split the base image into smaller images for each grid cell
void split_grid_into_images(SDL_Surface* image, int min_x, int max_x, int min_y, int max_y, int num_rows, int num_cols) {
    int cell_width = (max_x - min_x + 1) / num_cols;
    int cell_height = (max_y - min_y + 1) / num_rows;

    const char* cells_directory = "data/cells";

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            int start_x = min_x + col * cell_width;
            int start_y = min_y + row * cell_height;

            // Create a surface for the sub-image
            SDL_Rect src_rect = { start_x, start_y, cell_width, cell_height };
            SDL_Surface* cell_surface = SDL_CreateRGBSurface(0, cell_width, cell_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            SDL_BlitSurface(image, &src_rect, cell_surface, NULL);

            // Generate the filename for the sub-image
            char filename[256];
            snprintf(filename, sizeof(filename), "%s/cell_%d_%d.png", cells_directory, row, col);

            // Save the sub-image as a PNG file
            save_image(cell_surface, filename);

            // Free the surface of the sub-image
            SDL_FreeSurface(cell_surface);
        }
    }
}