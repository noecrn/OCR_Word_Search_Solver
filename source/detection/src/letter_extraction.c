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

// Function to extract the letters from the words list
void split_word_into_images(SDL_Surface* surface, int letter_left, int letter_right, int letter_top, int letter_bottom, int current_word, int current_letter) {
    int letter_width = letter_right - letter_left;
    int letter_height = letter_bottom - letter_top;

    const char* words_directory = "data/words";

    // Define the source rectangle for the letter within the main surface
    SDL_Rect src_rect = { letter_left, letter_top, letter_width, letter_height };

    // Create a surface to hold the letter image
    SDL_Surface* letter_surface = SDL_CreateRGBSurface(0, letter_width, letter_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (letter_surface == NULL) {
        fprintf(stderr, "Error creating letter surface: %s\n", SDL_GetError());
        exit(1);
    }

    // Copy the specified area from the main surface to the new letter surface
    SDL_BlitSurface(surface, &src_rect, letter_surface, NULL);

    // Generate the filename for the letter image
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/word_%d_letter_%d.png", words_directory, current_word, current_letter);

    // Save the letter image
    save_image(letter_surface, filename);

    // Free the surface of the letter image
    SDL_FreeSurface(letter_surface);
}