#include <SDL.h>
#include <stdio.h>

#include "../include/bounding_boxes.h"
#include "../include/rendering.h"

#define BLOCK_SIZE 5            // Size of the blocks to analyze
#define BLACK_TOLERANCE 1       // Tolerance for black pixel detection
#define WHITE_THRESHOLD 0       // Minimum number of black pixels to consider a block as black
#define SPACE_THRESHOLD 0       // Minimum number of consecutive white blocks to consider a gap

// Function to detect the number of columns in the grid
int detect_columns(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound) {
    int column_count = 0;           // Number of detected columns
    int in_black_column = 0;        // Indicator to know if we are in a column with black pixels

    // Parcour from left to right on the columns
    for (int x = left_bound; x <= right_bound; x += BLOCK_SIZE) {
        int black_pixel_count = 0;

        // Count the number of black pixels in this column
        for (int y = top_bound; y <= bottom_bound; y += BLOCK_SIZE) {
            black_pixel_count += count_black_pixels_in_block(surface, x, y, BLOCK_SIZE, BLACK_TOLERANCE);
        }

        // If this column contains black pixels
        if (black_pixel_count > WHITE_THRESHOLD) {

            // If we were not already in a black column, we count a new one
            if (!in_black_column) {
                column_count++;
                in_black_column = 1;
            }
        } else {
            in_black_column = 0;
        }
    }

    return column_count;
}

// Function to detect the number of rows in the grid
int detect_rows(SDL_Surface* surface, int left_bound, int right_bound, int top_bound, int bottom_bound) {
    int row_count = 0;          // Number of detected rows
    int in_black_row = 0;       // Indicator to know if we are in a row with black pixels

    // Parcour from top to bottom on the rows
    for (int y = top_bound; y <= bottom_bound; y += BLOCK_SIZE) {
        int black_pixel_count = 0;

        // Count the number of black pixels in this row
        for (int x = left_bound; x <= right_bound; x += BLOCK_SIZE) {
            black_pixel_count += count_black_pixels_in_block(surface, x, y, BLOCK_SIZE, BLACK_TOLERANCE);
        }

        // If this row contains black pixels
        if (black_pixel_count > WHITE_THRESHOLD) {
            
            // If we were not already in a black row, we count a new one
            if (!in_black_row) {
                row_count++;
                in_black_row = 1;
            }
        } else {
            in_black_row = 0;
        }
    }

    return row_count;
}

// Function to analyze the grid and detect the number of columns and rows
void analyze_grid(SDL_Surface* surface, int* left_bound, int* right_bound, int* top_bound, int* bottom_bound, int* column_count, int* row_count) {
    *column_count = detect_columns(surface, *left_bound, *right_bound, *top_bound, *bottom_bound);
    *row_count = detect_rows(surface, *left_bound, *right_bound, *top_bound, *bottom_bound);
}