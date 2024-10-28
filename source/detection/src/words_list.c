#include <SDL2/SDL.h>
#include "../include/bounding_boxes.h"
#include "../include/rendering.h"

// Function to adjust the borders of the words list
void adjust_border (
    SDL_Surface* surface,
    int* list_left,
    int* list_right,
    int* list_top,
    int* list_bottom, 
    int block_size, 
    int black_tolerance, 
    int white_threshold,
    int space_threshold
) {
    // Define starting points for propagation
    int start_x = *list_left;
    int start_y = *list_top;

    int consecutive_white_blocks = 0;

    // Propagation to the right
    for (int y = start_y; y < *list_bottom; y += block_size) {
        for (int x = start_x; x < surface->w; x += block_size) {
            int black_pixel_count = count_black_pixels_in_block(surface, x, y, block_size, black_tolerance, 0);
    
            if (black_pixel_count > white_threshold) { // If there are enough black pixels
                consecutive_white_blocks = 0;
                if (x + block_size < surface->w) { // Check if we are within bounds
                    *list_right = x + block_size > *list_right ? x + block_size : *list_right;
                } else { // If we are at the edge of the image
                    *list_right = x > *list_right ? x : *list_right;
                }
            } else { // If there are not enough black pixels
                consecutive_white_blocks++;
                if (consecutive_white_blocks >= space_threshold) break; // Stop if we have encountered enough consecutive white blocks
            }
        }
        consecutive_white_blocks = 0;
    }
}

// Function to get the words list bounding box
void detect_words_list (
    SDL_Surface* surface,
    int* list_left,
    int* list_right,
    int* list_top,
    int* list_bottom,
    int start_x,
    int start_y,
    int block_size, 
    int black_tolerance, 
    int white_threshold,
    int space_threshold,
    int left,
    int right,
    int top,
    int bottom
) {
    // Init bounds
    int width = surface->w;
    int height = surface->h;

    // Define starting points for propagation
    int start_points[][2] = {
        {start_x, start_y}
    };

    // Propagation to the left
    int num_start_points = sizeof(start_points) / sizeof(start_points[0]);

    if (right) {
        for (int i = 0; i < num_start_points; i++) {
            int start_x = start_points[i][0];
            int start_y = start_points[i][1];

            int consecutive_white_blocks = 0;
            
            for (int x = start_x; x >= 0; x -= block_size) {
                int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance, 0);

                if (black_pixel_count > white_threshold) { // If there are enough black pixels
                    consecutive_white_blocks = 0;
                    if (x > 0) { // Check if we are still in the image bounds
                        *list_left = x;
                    }
                } else { // If there are not enough black pixels
                    consecutive_white_blocks++;
                    if (consecutive_white_blocks > space_threshold) { //! > and not >=
                        break; // Stop if we have encountered enough consecutive white blocks
                    }
                }
            }
        }
    } else {
        *list_left = start_x;
    }

    // Propagation to the right
    if (left) {
        for (int i = 0; i < num_start_points; i++) {
            int start_x = start_points[i][0];
            int start_y = start_points[i][1];

            int consecutive_white_blocks = 0;

            consecutive_white_blocks = 0;
            for (int x = start_x; x < width; x += block_size) {
                int black_pixel_count = count_black_pixels_in_block(surface, x, start_y, block_size, black_tolerance, 0);

                if (black_pixel_count > white_threshold) { // If there are enough black pixels
                    consecutive_white_blocks = 0;
                    if (x + block_size < width) { // Check if we are within bounds
                        *list_right = x + block_size > *list_right ? x + block_size : *list_right;
                    } else { // If we are at the edge of the image
                        *list_right = x > *list_right ? x : *list_right;
                    }
                } else { // If there are not enough black pixels
                    consecutive_white_blocks++;
                    if (consecutive_white_blocks >= space_threshold) break; // Stop if we have encountered enough consecutive white blocks
                }
            }
        }
    } else {
        *list_right = start_x + block_size;
    }

    // Propagation to the top
    if (bottom) {
        for (int i = 0; i < num_start_points; i++) {
            int start_x = start_points[i][0];
            int start_y = start_points[i][1];

            int consecutive_white_blocks = 0;

            consecutive_white_blocks = 0;
            for (int y = start_y; y >= 0; y -= block_size) {
                int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance, 0);

                if (black_pixel_count > white_threshold) { // If there are enough black pixels
                    consecutive_white_blocks = 0;
                    if (y > 0) { // Check if we are still in the image bounds
                        *list_top = y;
                    } else {
                        *list_top = 0;
                    }
                } else { // If there are not enough black pixels
                    consecutive_white_blocks++;
                    if (consecutive_white_blocks >= space_threshold) break; // Stop if we have encountered enough consecutive white blocks
                }
            }
        }
    } else {
        *list_top = start_y;
    }

    // Propagation to the bottom
    if (top) {
        for (int i = 0; i < num_start_points; i++) {
            int start_x = start_points[i][0];
            int start_y = start_points[i][1];

            int consecutive_white_blocks = 0;

            consecutive_white_blocks = 0;
            for (int y = start_y; y < height; y += block_size) {
                int black_pixel_count = count_black_pixels_in_block(surface, start_x, y, block_size, black_tolerance, 0);

                if (black_pixel_count > white_threshold) { // If there are enough black pixels
                    consecutive_white_blocks = 0;
                    if (y < height) { // Check if we are still in the image bounds
                        if (y + block_size < height) { // Check if we are within bounds
                            *list_bottom = y + block_size > *list_bottom ? y + block_size : *list_bottom;
                        } else { // If we are at the edge of the image
                            *list_bottom = y > *list_bottom ? y : *list_bottom;
                        }
                    }
                } else { // If there are not enough black pixels
                    consecutive_white_blocks++;
                    if (consecutive_white_blocks >= space_threshold) break; // Stop if we have encountered enough consecutive white blocks
                }
            }
        }
    } else {
        *list_bottom = start_y + block_size;
    }

    adjust_border(surface, list_left, list_right, list_top, list_bottom, 17, black_tolerance, white_threshold, space_threshold);
    // draw_line(surface, *list_left, -1, (SDL_Color){255, 0, 0, 255});
    // draw_line(surface, -1, *list_top, (SDL_Color){255, 0, 0, 255});
    // draw_line(surface, *list_right, -1, (SDL_Color){255, 0, 0, 255});
    // draw_line(surface, -1, *list_bottom, (SDL_Color){255, 0, 0, 255});
}

// Function to find the words list in the grid
void find_words_list (
    SDL_Surface* surface, 
    int* grid_left,
    int* grid_right,
    int* grid_top,
    int* grid_bottom,
    int* list_left,
    int* list_right,
    int* list_top,
    int* list_bottom,
    int block_size, 
    int black_tolerance, 
    int white_threshold
) {
    // Init bounds
    int width = surface->w;
    int height = surface->h;

    // Calculate the center of the image
    int width_center = width / 2;
    int height_center = height / 2;

    // Propagation to the left
    for (int x = *grid_left - block_size; x >= 0; x -= block_size) {
        int black_pixel_count = count_black_pixels_in_block(surface, x, height_center, block_size, black_tolerance, 0);

        if (black_pixel_count > white_threshold) {
            detect_words_list(surface, list_left, list_right, list_top, list_bottom, x, height_center, 16, black_tolerance, white_threshold, 1, 0, 1, 1, 1);
            break;
        }
    }

    // Propagation to the right
    for (int x = *grid_right; x < width; x += block_size) {
        int black_pixel_count = count_black_pixels_in_block(surface, x, height_center, block_size, black_tolerance, 0);

        if (black_pixel_count > white_threshold) {
            detect_words_list(surface, list_left, list_right, list_top, list_bottom, x, height_center, 16, black_tolerance, white_threshold, 1, 1, 0, 1, 1);
            break;
        }
    }

    // Propagation to the top
    for (int y = *grid_top - block_size; y >= 0; y -= block_size) {
        int black_pixel_count = count_black_pixels_in_block(surface, width_center, y, block_size, black_tolerance, 0);

        if (black_pixel_count > white_threshold) {
            detect_words_list(surface, list_left, list_right, list_top, list_bottom, width_center, y, 16, black_tolerance, white_threshold, 1, 1, 1, 0, 1);
            break;
        }
    }

    // Propagation to the bottom
    for (int y = *grid_bottom; y < height; y += block_size) {
        int black_pixel_count = count_black_pixels_in_block(surface, width_center, y, block_size, black_tolerance, 0);

        if (black_pixel_count > white_threshold) {
            detect_words_list(surface, list_left, list_right, list_top, list_bottom, width_center, y, 16, black_tolerance, white_threshold, 2, 1, 1, 1, 0);
            break;
        }
    }
}