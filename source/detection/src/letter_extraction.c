#include <SDL2/SDL.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/image_processing.h"

// Function to split the base image into smaller images for each grid cell
void split_grid_into_images(SDL_Surface* image, int left, int top, int right, int bottom, int num_rows, int num_cols) {
    float cell_width = (float)(right - left) / num_cols;
    float cell_height = (float)(bottom - top) / num_rows;

    const char* cells_directory = "data/cells";

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            int start_x = left + (int)(col * cell_width);
            int start_y = top + (int)(row * cell_height);

            // Create a surface for the sub-image
            SDL_Rect src_rect = { start_x, start_y, (int)cell_width, (int)cell_height };
            SDL_Surface* cell_surface = SDL_CreateRGBSurface(0, (int)cell_width, (int)cell_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
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