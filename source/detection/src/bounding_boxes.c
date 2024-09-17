#include <SDL.h>
#include <stdio.h>
#include "../include/rendering.h"

// Function to check if a pixel is black based on a tolerance value
int is_black_pixel(Uint8 r, Uint8 g, Uint8 b, int black_tolerance) {
    return r < black_tolerance && g < black_tolerance && b < black_tolerance;
}

// Function to count the number of black pixels in a block
int count_black_pixels_in_block(SDL_Surface* surface, int start_x, int start_y, int block_size, int black_tolerance) {
    int count_black = 0;

    // Loop through the block
    for (int y = start_y; y < start_y + block_size && y < surface->h; y++) {
        for (int x = start_x; x < start_x + block_size && x < surface->w; x++) {
            Uint32 pixel = 0;
            Uint8 r, g, b, a;
            pixel = ((Uint32*)surface->pixels)[y * surface->pitch / 4 + x];
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            if (is_black_pixel(r, g, b, black_tolerance)) {
                count_black++;
            }
        }
    }

    // Draw the border around the block
    // draw_square(surface, start_x, start_y, block_size, (SDL_Color){0, 0, 255, 255});

    return count_black;
}

// Function to detect the bounding box of letters in a grid
void detect_letter_grid(
    SDL_Surface* surface, 
    int* left_bound, 
    int* right_bound, 
    int* top_bound, 
    int* bottom_bound, 
    int block_size, 
    int black_tolerance, 
    int white_threshold, 
    int space_threshold
) {
    // Init bounds
    int width = surface->w;
    int height = surface->h;

    // Init bounds to extreme values
    *left_bound = width;
    *right_bound = 0;
    *top_bound = height;
    *bottom_bound = 0;

    // Calculate the center of the image
    int width_center = width / 2;
    int height_center = height / 2 + block_size * 3;

    // Define starting points for propagation
    int start_points[][2] = {
        {width_center, height_center}, 
        {width_center + block_size, height_center + block_size}, 
        {width_center - block_size, height_center + block_size}, 
        {width_center + block_size, height_center - block_size}, 
        {width_center - block_size, height_center - block_size}
    };

    // Propagation to the left
    int num_start_points = sizeof(start_points) / sizeof(start_points[0]);

    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;
        
        for (int x = start_x; x >= 0; x -= block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) { // If there are enough black pixels
                consecutive_white_blocks = 0;
                if (x < *left_bound) { // Check if we are within bounds
                    *left_bound = x;
                }
            } else { // If there are not enough black pixels
                consecutive_white_blocks++;
                if (consecutive_white_blocks > space_threshold) { //! > and not >=
                    break; // Stop if we have encountered enough consecutive white blocks
                }
            }
        }
    }

    // Propagation to the right
    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        consecutive_white_blocks = 0;
        for (int x = start_x; x < width; x += block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) { // If there are enough black pixels
                consecutive_white_blocks = 0;
                if (x + block_size < width) { // Check if we are within bounds
                    *right_bound = x + block_size > *right_bound ? x + block_size : *right_bound;
                } else { // If we are at the edge of the image
                    *right_bound = x > *right_bound ? x : *right_bound;
                }
            } else { // If there are not enough black pixels
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break; // Stop if we have encountered enough consecutive white blocks
            }
        }
    }

    // Propagation to the top
    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        consecutive_white_blocks = 0;
        for (int y = start_y; y >= 0; y -= block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) { // If there are enough black pixels
                consecutive_white_blocks = 0;
                if (y < *top_bound) { // Check if we are within bounds
                    *top_bound = y;
                }
            } else { // If there are not enough black pixels
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break; // Stop if we have encountered enough consecutive white blocks
            }
        }
    }

    // Propagation to the bottom
    for (int i = 0; i < num_start_points; i++) {
        int start_x = start_points[i][0];
        int start_y = start_points[i][1];

        int consecutive_white_blocks = 0;

        consecutive_white_blocks = 0;
        for (int y = start_y; y < height; y += block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance);

            if (black_pixel_count > white_threshold) { // If there are enough black pixels
                consecutive_white_blocks = 0;
                if (y + block_size < height) { // Check if we are within bounds
                    *bottom_bound = y + block_size > *bottom_bound ? y + block_size : *bottom_bound;
                } else { // If we are at the edge of the image
                    *bottom_bound = y > *bottom_bound ? y : *bottom_bound;
                }
            } else { // If there are not enough black pixels
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break; // Stop if we have encountered enough consecutive white blocks
            }
        }
    }
}