#include "../../include/bounding_boxes.h"
#include "../../include/image_processing.h"
#include "../../include/rendering.h"
#include "../../include/words_extraction.h"
#include <SDL2/SDL.h>

// Helper function to check if a pixel at position (x,y) is black
static inline int check_black_pixel(SDL_Surface *surface, int x, int y,
                                    int tolerance) {
  if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
    return 0;

  Uint8 *pixels = surface->pixels;
  int bpp = surface->format->BytesPerPixel;
  Uint8 r, g, b;
  Uint32 pixel = *(Uint32 *)(pixels + y * surface->pitch + x * bpp);
  SDL_GetRGB(pixel, surface->format, &r, &g, &b);
  return is_black_pixel(r, g, b, tolerance);
}

// Function to adjust the borders of the words list
void adjust_border(SDL_Surface *surface, int *list_left, int *list_right,
                   int *list_top, int *list_bottom, int black_tolerance) {
  // Adjust left border
  int x = *list_left;
  while (x > 6) { // Need at least 6 pixels for gap checking
    int has_black = 0;
    // Check current position
    for (int y = *list_top; y <= *list_bottom; y++) {
      if (check_black_pixel(surface, x, y, black_tolerance)) {
        has_black = 1;
        break;
      }
    }

    if (!has_black) {
      // Check 6 pixels ahead for potential continuation
      int has_black_after = 0;
      for (int y = *list_top; y <= *list_bottom; y++) {
        if (check_black_pixel(surface, x - 6, y, black_tolerance)) {
          has_black_after = 1;
          break;
        }
      }
      if (!has_black_after)
        break; // No black pixels found even after gap
      x -= 6;  // Skip to the next potential black pixels
    }
    x--;
    *list_left = x;
  }

  // Adjust right border
  x = *list_right;
  while (x < surface->w - 7) { // Need space for 6-pixel gap checking
    int has_black = 0;
    for (int y = *list_top; y <= *list_bottom; y++) {
      if (check_black_pixel(surface, x, y, black_tolerance)) {
        has_black = 1;
        break;
      }
    }

    if (!has_black) {
      int has_black_after = 0;
      for (int y = *list_top; y <= *list_bottom; y++) {
        if (check_black_pixel(surface, x + 6, y, black_tolerance)) {
          has_black_after = 1;
          break;
        }
      }
      if (!has_black_after)
        break;
      x += 6;
    }
    x++;
    *list_right = x;
  }

  // Adjust top border
  int y = *list_top;
  while (y > 6) {
    int has_black = 0;
    for (x = *list_left; x <= *list_right; x++) {
      if (check_black_pixel(surface, x, y, black_tolerance)) {
        has_black = 1;
        break;
      }
    }

    if (!has_black) {
      int has_black_after = 0;
      for (x = *list_left; x <= *list_right; x++) {
        if (check_black_pixel(surface, x, y - 6, black_tolerance)) {
          has_black_after = 1;
          break;
        }
      }
      if (!has_black_after)
        break;
      y -= 6;
    }
    y--;
    *list_top = y;
  }

  // Adjust bottom border
  y = *list_bottom;
  while (y < surface->h - 7) {
    int has_black = 0;
    for (x = *list_left; x <= *list_right; x++) {
      if (check_black_pixel(surface, x, y, black_tolerance)) {
        has_black = 1;
        break;
      }
    }

    if (!has_black) {
      int has_black_after = 0;
      for (x = *list_left; x <= *list_right; x++) {
        if (check_black_pixel(surface, x, y + 6, black_tolerance)) {
          has_black_after = 1;
          break;
        }
      }
      if (!has_black_after)
        break;
      y += 6;
    }
    y++;
    *list_bottom = y;
  }
}

// Function to get the words list bounding box
void detect_words_list(SDL_Surface *surface, int *list_left, int *list_right,
                       int *list_top, int *list_bottom, int start_x,
                       int start_y, int block_size, int black_tolerance,
                       int white_threshold, int space_threshold, int left,
                       int right, int top, int bottom) {
  // Init bounds
  int width = surface->w;
  int height = surface->h;

  // Define starting points for propagation
  int start_points[][2] = {{start_x, start_y}};

  // Propagation to the left
  int num_start_points = sizeof(start_points) / sizeof(start_points[0]);

  if (right) {
    for (int i = 0; i < num_start_points; i++) {
      int start_x = start_points[i][0];
      int start_y = start_points[i][1];

      int consecutive_white_blocks = 0;

      for (int x = start_x; x >= 0; x -= block_size) {
        int black_pixel_count = count_black_pixels_in_block(
            surface, x, start_y, block_size, black_tolerance, 0);

        if (black_pixel_count >
            white_threshold) { // If there are enough black pixels
          consecutive_white_blocks = 0;
          if (x > 0) { // Check if we are still in the image bounds
            *list_left = x;
          }
        } else { // If there are not enough black pixels
          consecutive_white_blocks++;
          if (consecutive_white_blocks > space_threshold) { //! > and not >=
            break; // Stop if we have encountered enough consecutive white
                   // blocks
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
        int black_pixel_count = count_black_pixels_in_block(
            surface, x, start_y, block_size, black_tolerance, 0);

        if (black_pixel_count >
            white_threshold) { // If there are enough black pixels
          consecutive_white_blocks = 0;
          if (x + block_size < width) { // Check if we are within bounds
            *list_right =
                x + block_size > *list_right ? x + block_size : *list_right;
          } else { // If we are at the edge of the image
            *list_right = x > *list_right ? x : *list_right;
          }
        } else { // If there are not enough black pixels
          consecutive_white_blocks++;
          if (consecutive_white_blocks >= space_threshold)
            break; // Stop if we have encountered enough consecutive white
                   // blocks
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
        int black_pixel_count = count_black_pixels_in_block(
            surface, start_x, y, block_size, black_tolerance, 0);

        if (black_pixel_count >
            white_threshold) { // If there are enough black pixels
          consecutive_white_blocks = 0;
          if (y > 0) { // Check if we are still in the image bounds
            *list_top = y;
          } else {
            *list_top = 0;
          }
        } else { // If there are not enough black pixels
          consecutive_white_blocks++;
          if (consecutive_white_blocks >= space_threshold)
            break; // Stop if we have encountered enough consecutive white
                   // blocks
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
        int black_pixel_count = count_black_pixels_in_block(
            surface, start_x, y, block_size, black_tolerance, 0);

        if (black_pixel_count >
            white_threshold) { // If there are enough black pixels
          consecutive_white_blocks = 0;
          if (y < height) { // Check if we are still in the image bounds
            if (y + block_size < height) { // Check if we are within bounds
              *list_bottom =
                  y + block_size > *list_bottom ? y + block_size : *list_bottom;
            } else { // If we are at the edge of the image
              *list_bottom = y > *list_bottom ? y : *list_bottom;
            }
          }
        } else { // If there are not enough black pixels
          consecutive_white_blocks++;
          if (consecutive_white_blocks >= space_threshold)
            break; // Stop if we have encountered enough consecutive white
                   // blocks
        }
      }
    }
  } else {
    *list_bottom = start_y + block_size;
  }

  adjust_border(surface, list_left, list_right, list_top, list_bottom,
                black_tolerance);
}

// Function to find the words list in the grid
// Modify function signature to use a pointer to current_word
void find_words_list(SDL_Surface *surface, int *grid_left, int *grid_right,
                     int *grid_top, int *grid_bottom, int *list_left,
                     int *list_right, int *list_top, int *list_bottom,
                     int block_size, int black_tolerance, int white_threshold,
                     int flag, int *current_word) {
  // Initialize list bounds to prevent undefined behavior
  *list_left = 0;
  *list_right = 0;
  *list_top = 0;
  *list_bottom = 0;

  // Init bounds with bounds checking
  int width = surface->w;
  int height = surface->h;

  int is_found = 0;

  // Validate grid bounds
  if (*grid_left < 0 || *grid_right >= width || *grid_top < 0 ||
      *grid_bottom >= height) {
    printf("[ERROR] Invalid grid bounds\n");
    return;
  }

  // Calculate the center of the image
  int width_center = width / 2;
  int _height_center = height / 2 - block_size;
  int height_center = height / 2;

  // Propagation to the left
  for (int x = *grid_left - block_size; x >= 0; x -= block_size) {
    if (x < 0 || height_center - block_size < 0) {
      continue;
    }
    int black_pixel_count = count_black_pixels_in_block(
        surface, x, _height_center, block_size, black_tolerance, 0);

    if (black_pixel_count > white_threshold) {
      if (x - 2 * block_size < 0) {
        printf("[ERROR] Invalid words list bounds detected\n");
        break;
      }
      detect_words_list(surface, list_left, list_right, list_top, list_bottom,
                        x - 2 * block_size, _height_center, 17, black_tolerance,
                        white_threshold, 2, 0, 1, 1, 1);
      break;
    }
  }

  // Propagation to the right
  for (int x = *grid_right; x < width; x += block_size) {
    if (x >= width || height_center < 0) {
      continue;
    }
    int black_pixel_count = count_black_pixels_in_block(
        surface, x, height_center, block_size, black_tolerance, 0);

    if (black_pixel_count > white_threshold) {
      detect_words_list(surface, list_left, list_right, list_top, list_bottom,
                        x, height_center, 25, black_tolerance, white_threshold,
                        1, 1, 0, 1, 1);
      break;
    }
  }

  // Propagation to the top
  for (int y = *grid_top - block_size; y >= 0; y -= block_size) {
    if (width_center + 5 >= width || y < 0) {
      continue;
    }
    int black_pixel_count = count_black_pixels_in_block(
        surface, width_center + 5, y, block_size, black_tolerance, 0);

    if (black_pixel_count > white_threshold) {
      is_found = 1;
      detect_words_list(surface, list_left, list_right, list_top, list_bottom,
                        width_center, y, 19, black_tolerance, white_threshold,
                        1, 1, 1, 0, 1);
      break;
    }
  }

  // Propagation to the bottom
  for (int y = *grid_bottom; y < height; y += block_size) {
    if (y >= height || width_center < 0) {
      continue;
    }
    int black_pixel_count = count_black_pixels_in_block(
        surface, width_center - 20, y, block_size, black_tolerance, 0);

    if (black_pixel_count > white_threshold) {
      is_found = 1;
      detect_words_list(surface, list_left, list_right, list_top, list_bottom,
                        width_center - block_size, y, 16, black_tolerance,
                        white_threshold, 2, 1, 1, 1, 0);
      break;
    }
  }

  // Validate final bounds
  if (*list_left < 0 || *list_right >= width || *list_top < 0 ||
      *list_bottom >= height) {
    printf("[ERROR] Invalid words list bounds detected\n");
    *list_left = 0;
    *list_right = width - 1;
    *list_top = 0;
    *list_bottom = height - 1;
  }

  // Find the other words list (right and left)
  if (is_found) {
    int list_left2 = 0;
    int list_right2 = 0;
    int list_top2 = 0;
    int list_bottom2 = 0;

    // Propagation to the left
    for (int x = *list_left - block_size; x >= 0; x -= block_size) {
      if (x < 0 || *list_top < 0) {
        continue;
      }
      int black_pixel_count = count_black_pixels_in_block(
          surface, x, *list_top, block_size, black_tolerance, 0);

      if (black_pixel_count > white_threshold) {
        detect_words_list(surface, &list_left2, &list_right2, &list_top2,
                          &list_bottom2, x - 40, *list_top, 26, black_tolerance,
                          white_threshold, 2, 0, 1, 1, 0);
        list_bottom2 = *list_bottom;
        break;
      }
    }

    int list_left3 = 0;
    int list_right3 = 0;
    int list_top3 = 0;
    int list_bottom3 = 0;

    // Propagation to the right
    for (int x = *list_right; x < width; x += block_size) {
      if (x >= width || *list_top < 0) {
        continue;
      }
      int black_pixel_count = count_black_pixels_in_block(
          surface, x, *list_top, block_size, black_tolerance, 0);

      if (black_pixel_count > white_threshold) {
        detect_words_list(surface, &list_left3, &list_right3, &list_top3,
                          &list_bottom3, x, *list_top, 25, black_tolerance,
                          white_threshold, 1, 1, 0, 1, 0);
        list_bottom3 = *list_bottom;
        break;
      }
    }

    if (flag) {
      SDL_Color blue = {0, 0, 255, 255};

      draw_horizontal_line(surface, list_top2, list_left2, list_right2, blue);
      draw_horizontal_line(surface, list_bottom2, list_left2, list_right2,
                           blue);
      draw_vertical_line(surface, list_left2, list_top2, list_bottom2, blue);
      draw_vertical_line(surface, list_right2, list_top2, list_bottom2, blue);

      draw_horizontal_line(surface, list_top3, list_left3, list_right3, blue);
      draw_horizontal_line(surface, list_bottom3, list_left3, list_right3,
                           blue);
      draw_vertical_line(surface, list_left3, list_top3, list_bottom3, blue);
      draw_vertical_line(surface, list_right3, list_top3, list_bottom3, blue);
    }

    // Parse the other words list (right and left)
    int word_count = count_words(surface, list_left2, list_right2, list_top2,
                                 list_bottom2, 0);

    // Extract the words list
    coordinates *words = NULL;
    words = words_extraction(surface, list_left2, list_right2, list_top2,
                             list_bottom2, 1, word_count, flag);

    if (!words) {
      printf("[ERROR] Failed to extract words\n");
      goto cleanup;
    }

    if (word_count <= 0) {
      printf("[ERROR] No words found in the list\n");
      goto cleanup;
    }

    // Extract and resize letters
    for (int i = 0; i < word_count; i++) {
      int temp = letters_extraction(surface, list_left2, list_right2,
                                    words[i].top_bound, words[i].bottom_bound,
                                    0, *current_word);
      if (temp > 0) {
        letters_resize(*current_word, temp);
        (*current_word)++; // Increment pointer value
      } else {
        printf("[WARNING] No letters extracted for word %d\n", i + 1);
      }
    }

    // Parse the other words list (right and left)
    word_count = count_words(surface, list_left3, list_right3, list_top3,
                             list_bottom3, 0);

    // Extract the words list
    words = words_extraction(surface, list_left3, list_right3, list_top3,
                             list_bottom3, 1, word_count, flag);

    if (word_count <= 0) {
      printf("[ERROR] No words found in the list\n");
      goto cleanup;
    }

    for (int i = 0; i < word_count; i++) {
      int temp = letters_extraction(surface, list_left3, list_right3,
                                    words[i].top_bound, words[i].bottom_bound,
                                    0, *current_word);
      if (temp > 0) {
        letters_resize(*current_word, temp);
        (*current_word)++; // Increment pointer value
      } else {
        printf("[WARNING] No letters extracted for word %d\n", i + 1);
      }
    }
  }

  if (flag) {
    SDL_Color blue = {0, 0, 255, 255};
    draw_horizontal_line(surface, *list_top, *list_left, *list_right, blue);
    draw_horizontal_line(surface, *list_bottom, *list_left, *list_right, blue);
    draw_vertical_line(surface, *list_left, *list_top, *list_bottom, blue);
    draw_vertical_line(surface, *list_right, *list_top, *list_bottom, blue);
  }

cleanup:
  // Any cleanup code here if needed
  return;
}