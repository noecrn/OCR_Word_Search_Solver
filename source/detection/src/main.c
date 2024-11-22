#include "../include/border_dilatation.h"
#include "../include/bounding_boxes.h"
#include "../include/grid_detection.h"
#include "../include/hough_transform.h"
#include "../include/image_loader.h"
#include "../include/image_processing.h"
#include "../include/lettre_extraction.h"
#include "../include/line_detection.h"
#include "../include/rendering.h"
#include "../include/words_extraction.h"
#include "../include/words_list.h"
#include <stdio.h>
#include <time.h>

// Function to get image parameters based on the input path
void image_parameter(const char *inputPath, SDL_Surface *surface,
                     int *grid_left, int *grid_right, int *grid_top,
                     int *grid_bottom) {
  if (strcmp(inputPath, "data/level_4_image_2.png") == 0) {
    detect_letter_grid(surface, grid_left, grid_right, grid_top, grid_bottom,
                       15, 1, 0, 2);
  } else if (strcmp(inputPath, "data/level_3_image_1.png") == 0) {
    detect_letter_grid(surface, grid_left, grid_right, grid_top, grid_bottom,
                       15, 1, 0, 3);
  } else if (strcmp(inputPath, "data/level_4_image_1.png") == 0) {
    detect_letter_grid(surface, grid_left, grid_right, grid_top, grid_bottom,
                       15, 1, 0, 5);
  } else {
    detect_letter_grid(surface, grid_left, grid_right, grid_top, grid_bottom,
                       15, 1, 0, 4);
  }
}

// Function to detect grid and words list without grid
int no_grid_detection(SDL_Surface *image, const char *IMAGE) {
  // Declare variables
  int grid_left, grid_right, grid_top, grid_bottom;
  int list_left, list_right, list_top, list_bottom;
  int num_rows, num_cols;

  // Get the bounding box parameters
  image_parameter(IMAGE, image, &grid_left, &grid_right, &grid_top,
                  &grid_bottom);

  // Adjust the borders
  check_and_dilate_borders(image, &grid_left, &grid_right, &grid_top,
                           &grid_bottom);

  // Count the number of rows and columns
  analyze_grid(image, &grid_left, &grid_right, &grid_top, &grid_bottom,
               &num_cols, &num_rows);

  // Draw the grid
  draw_grid(image, grid_left, grid_right, grid_top, grid_bottom, num_rows,
            num_cols);

  // Detect the word list
  find_words_list(image, &grid_left, &grid_right, &grid_top, &grid_bottom,
                  &list_left, &list_right, &list_top, &list_bottom, 15, 1, 0);

  // Count the number of words
  int word_count =
      count_words(image, list_left, list_right, list_top, list_bottom, 0);

  // Extract the words list
  coordinates *words = words_extraction(image, list_left, list_right, list_top,
                                        list_bottom, 1, word_count);

  // Extract the letters
  int temp = 0;

  for (int i = 0; i < word_count; i++) {
    temp = letters_extraction(image, list_left, list_right, words[i].top_bound,
                              words[i].bottom_bound, 1, i);

    // Resize the letters
    letters_resize(i, temp);
  }

  // Save the SDL surfare to .png`
  save_image(image, "output/output.png");

  // Split the image into smaller images
  split_grid_into_images(image, grid_left, grid_top, grid_right, grid_bottom,
                         num_rows, num_cols);

  // Resize cells
  cells_resize(num_rows, num_cols);

  // Display the image
  // display_image(image);

  // Clean up
  SDL_FreeSurface(image);
  SDL_Quit();

  return 0;
}

int with_grid_detection(SDL_Surface *image) {
  GridCoords coordinates = detect_grid_coords(image);

  if (coordinates.left == 0 && coordinates.right == 0 && coordinates.top == 0 &&
      coordinates.bottom == 0) {
    printf("No grid detected\n");
    return 0;
  } else {
    printf("Grid detected\n");

    // Display the grid
    SDL_Color red = {255, 0, 0, 255};
    draw_line(image, coordinates.left, -1, red);
    draw_line(image, coordinates.right, -1, red);
    draw_line(image, -1, coordinates.top, red);
    draw_line(image, -1, coordinates.bottom, red);

    // Save the SDL surfare to .png
    save_image(image, "output/output.png");

    return 1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  // Start the clock
  clock_t start_time = clock();
  if (argc < 2 || argc > 3) {
    printf("Usage: %s <image_path> or %s <image_path> <angle>\n", argv[0],
           argv[0]);
    return 1;
  } else if (argc == 2) { // grid detection function
    const char *IMAGE = argv[1];

    // Try to load the image first to check if it exists
    SDL_Surface *test_image = IMG_Load(IMAGE);
    if (!test_image) {
        printf("Error: Could not load image '%s': %s\n", IMAGE, IMG_GetError());
        return 1;
    }
    SDL_FreeSurface(test_image);

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Resize the image
    resize_image(IMAGE, "output/temp_resized.png", 700);

    // Load the resized image
    SDL_Surface *image = load_image("output/temp_resized.png");

    // Check if a grid is detected in the image
    if (detect_grid(image) == 0) {
      printf("[DEBUG] Using no_grid_detection method\n");
      no_grid_detection(image, IMAGE);
    } else {
      printf("[DEBUG] Attempting Hough transform detection\n");
      if (with_grid_detection(image) == 0) {
        printf("[DEBUG] Hough transform failed to detect grid. Stopping.\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
      }
    }
  } else if (argc == 3) { // rotate function
    printf("Rotate function\n");
    const char *IMAGE = argv[1];
    const char *ANGLE = argv[2];

    rotate(IMAGE, "output/rotated.png", atof(ANGLE));

    return 0;
  }

  // Calculate the execution time
  clock_t end_time = clock();
  double execute_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  printf("Execution time: %f seconds\n", execute_time);
}
