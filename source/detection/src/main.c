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
  if (!image) {
    return 1;
  }

  // Declare variables
  int grid_left = 0, grid_right = 0, grid_top = 0, grid_bottom = 0;
  int list_left = 0, list_right = 0, list_top = 0, list_bottom = 0;
  int num_rows = 0, num_cols = 0;

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

  // Free the words array
  free(words);

  // Save before freeing
  save_image(image, "output/output.png");

  // Split the image into smaller images
  split_grid_into_images(image, grid_left, grid_top, grid_right, grid_bottom,
                         num_rows, num_cols);

  // Resize cells
  cells_resize(num_rows, num_cols);

  return 0;
}

int with_grid_detection(SDL_Surface *image) {
  int result = 0;
  if (!image) {
    return result;
  }

  GridCoords coordinates = detect_grid_coords(image);

  if (coordinates.left == 0 && coordinates.right == 0 && coordinates.top == 0 &&
      coordinates.bottom == 0) {
    printf("No grid detected\n");
  } else {
    printf("Grid detected\n");
    SDL_Color red = {255, 0, 0, 255};
    draw_line(image, coordinates.left, -1, red);
    draw_line(image, coordinates.right, -1, red);
    draw_line(image, -1, coordinates.top, red);
    draw_line(image, -1, coordinates.bottom, red);
    save_image(image, "output/output_.png");
    result = 1;
  }

  return result;
}

int main(int argc, char *argv[]) {
  // Start the clock
  clock_t start_time = clock();

  if (argc < 2 || argc > 3) {
    printf("Usage: %s <image_path> or %s <image_path> <angle>\n", argv[0],
           argv[0]);
    return 1;
  }

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  if (argc == 2) { // grid detection function
    const char *IMAGE = argv[1];
    SDL_Surface *image = NULL;
    SDL_Surface *working_copy = NULL;
    int result = 1;

    printf("Processing image: %s\n", IMAGE);

    // Resize the image
    resize_image(IMAGE, "output/temp_resized.png", 700);

    // Load the resized image
    image = load_image("output/temp_resized.png");
    if (!image) {
      printf("Error loading image\n");
      goto cleanup;
    }

    printf("Image loaded: %dx%d pixels, %d bpp\n", image->w, image->h,
           image->format->BitsPerPixel);

    // Store grid detection result
    int has_grid = detect_grid(image);

    // Use copy of image for processing
    working_copy = SDL_ConvertSurface(image, image->format, 0);
    if (!working_copy) {
      goto cleanup;
    }

    if (has_grid == 0) {
      printf("No grid detected_\n");
      result = no_grid_detection(working_copy, IMAGE);
    } else {
      printf("Grid detected_\n");
      result = with_grid_detection(working_copy);
    }

  cleanup:
    if (working_copy) {
      SDL_FreeSurface(working_copy);
    }
    if (image) {
      SDL_FreeSurface(image);
    }
    SDL_Quit();
    return result;
  } else if (argc == 3) { // rotate function
    printf("Rotate function\n");
    const char *IMAGE = argv[1];
    const char *ANGLE = argv[2];
    rotate(IMAGE, "output/rotated.png", atof(ANGLE));
  }

  // Calculate the execution time
  clock_t end_time = clock();
  double execute_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  printf("Execution time: %f seconds\n", execute_time);

  SDL_Quit();
  return 0;
}
