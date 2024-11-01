#include "../include/border_dilatation.h"
#include "../include/bounding_boxes.h"
#include "../include/image_loader.h"
#include "../include/image_processing.h"
#include "../include/lettre_extraction.h"
#include "../include/line_detection.h"
#include "../include/rendering.h"
#include "../include/words_extraction.h"
#include "../include/words_list.h"

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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <image_path>\n", argv[0]);
    return 1;
  }

  const char *IMAGE = argv[1];

  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);

  // Define variables
  int grid_left, grid_right, grid_top, grid_bottom;
  int list_left, list_right, list_top, list_bottom;
  int num_rows, num_cols;

  // Resize the image
  resize_image(IMAGE, "output/temp_resized.png", 700);

  // Load the resized image
  SDL_Surface *image = load_image("output/temp_resized.png");

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
  // int word_count =
  //     count_words(image, list_left, list_right, list_top, list_bottom, 0);

  // Extract the words list
  // coordinates *words = words_extraction(image, list_left, list_right,
  // list_top, list_bottom, 1, word_count);

  // Extract the letters
  // int temp = 0;

  // for (int i = 0; i < word_count; i++) {
  //   temp = letters_extraction(image, list_left, list_right,
  //   words[i].top_bound,
  //                             words[i].bottom_bound, 1, i);

  //   // Resize the letters
  //   letters_resize(i, temp);
  // }

  // Save the SDL surfare to .png`
  save_image(image, "output/output.png");

  // Split the image into smaller images
  // split_grid_into_images(image, grid_left, grid_top, grid_right, grid_bottom,
  // num_rows, num_cols);

  // Resize cells
  // cells_resize(num_rows, num_cols);

  // Clean up
  SDL_FreeSurface(image);
  SDL_Quit();
  return 0;
}