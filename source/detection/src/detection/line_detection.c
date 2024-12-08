#include <SDL.h>
#include <stdio.h>

#include "../../include/bounding_boxes.h"
#include "../../include/rendering.h"

#define BLACK_TOLERANCE 1 // Tolerance for black pixel detection

typedef enum { DETECT_COLUMNS, DETECT_ROWS } DetectionMode;

// Generic function to detect lines (rows or columns)
static int detect_lines(SDL_Surface *surface, int left_bound, int right_bound,
                        int top_bound, int bottom_bound, DetectionMode mode,
                        int white_threshold, int block_size) {
  int line_count = 0;
  int in_black_line = 0;

  // Primary loop bounds
  int start = (mode == DETECT_COLUMNS) ? left_bound : top_bound;
  int end = (mode == DETECT_COLUMNS) ? right_bound : bottom_bound;

  // Secondary loop bounds
  int inner_start = (mode == DETECT_COLUMNS) ? top_bound : left_bound;
  int inner_end = (mode == DETECT_COLUMNS) ? bottom_bound : right_bound;

  for (int primary = start; primary <= end; primary += block_size) {
    int black_pixel_count = 0;

    for (int secondary = inner_start; secondary <= inner_end;
         secondary += block_size) {
      int x = (mode == DETECT_COLUMNS) ? primary : secondary;
      int y = (mode == DETECT_COLUMNS) ? secondary : primary;

      black_pixel_count += count_black_pixels_in_block(
          surface, x, y, block_size, BLACK_TOLERANCE, 0);
    }

    if (black_pixel_count > white_threshold) {
      if (!in_black_line) {
        line_count++;
        in_black_line = 1;
      }
    } else
      in_black_line = 0;
    // printf("black_pixel_count:%d, %d\n", in_black_line, black_pixel_count);
  }

  return line_count;
}

// Wrapper functions to maintain the original interface
int detect_columns(SDL_Surface *surface, int left_bound, int right_bound,
                   int top_bound, int bottom_bound, int white_threshold,
                   int block_size) {
  return detect_lines(surface, left_bound, right_bound, top_bound, bottom_bound,
                      DETECT_COLUMNS, white_threshold, block_size);
}

int detect_rows(SDL_Surface *surface, int left_bound, int right_bound,
                int top_bound, int bottom_bound, int white_threshold,
                int block_size) {
  return detect_lines(surface, left_bound, right_bound, top_bound, bottom_bound,
                      DETECT_ROWS, white_threshold, block_size);
}

// Function to analyze the grid and detect the number of columns and rows
void analyze_grid(SDL_Surface *surface, int *left_bound, int *right_bound,
                  int *top_bound, int *bottom_bound, int *column_count,
                  int *row_count, int white_threshold, int block_size) {
  // printf("Column\n");
  *column_count = detect_columns(surface, *left_bound, *right_bound, *top_bound,
                                 *bottom_bound, white_threshold, block_size);
  // printf("\nRow\n");
  *row_count = detect_rows(surface, *left_bound, *right_bound, *top_bound,
                           *bottom_bound, white_threshold, block_size);
}