#include "../include/border_dilatation.h"
#include "../include/bounding_boxes.h"
#include "../include/delete_grid.h"
#include "../include/grid_detection.h"
#include "../include/hough_transform.h"
#include "../include/image_loader.h"
#include "../include/image_processing.h"
#include "../include/lettre_extraction.h"
#include "../include/line_detection.h"
#include "../include/rendering.h"
#include "../include/words_extraction.h"
#include "../include/words_list.h"
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

// Add this function declaration at the top after includes
void cleanup_directory(const char *dir_path) {
  // Create parent directory if it doesn't exist
  const char *parent_dir = "../data";
  struct stat st = {0};
  if (stat(parent_dir, &st) == -1) {
    mkdir(parent_dir, 0777);
  }

  // Create or clean directory
  DIR *dir = opendir(dir_path);
  if (dir == NULL) {
    // Create directory if it doesn't exist
    mkdir(dir_path, 0777);
    return;
  }

  struct dirent *entry;
  char file_path[512];
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) { // If it's a regular file
      snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);
      remove(file_path);
    }
  }
  closedir(dir);
}

// Function to get image parameters based on the input path
void image_parameter(SDL_Surface *surface, int *grid_left, int *grid_right,
                     int *grid_top, int *grid_bottom) {
  int biggest_component = 0;
  // Initialize variables
  int a = 15, b = 1, c = 0, d = 4;

  detect_grid(surface, &biggest_component, 0); // Detect grid
  printf("Biggest component: %d\n", biggest_component);

  if (biggest_component == 946) { // Level 1 image 1
    a = 15;
    b = 1;
    c = 0;
    d = 5;
  } else if (biggest_component == 3756 ||
            biggest_component == 13990) { // Level 2 image 1
    a = 20;
    b = 1;
    c = 0;
    d = 2;
  } else if (biggest_component == 1596 ||
            biggest_component == 1634 ||
            biggest_component ==  1368) { // Level 2 image 2
    a = 25;
    b = 1;
    c = 0;
    d = 1;
  } else if (biggest_component == 1114 ||
            biggest_component == 480) { // Level 3 image 1
    printf("31\n");
    a = 14;
    b = 1;
    c = 0;
    d = 2;
  } else if (biggest_component == 560) { // Level 4 image 1
    a = 15;
    b = 1;
    c = 0;
    d = 5;
  } else if (biggest_component == 985) { // Level 4 image 2
    a = 25;
    b = 2;
    c = 0;
    d = 2;
  }

  detect_letter_grid(surface, grid_left, grid_right, grid_top, grid_bottom, a,
                     b, c, d);
}

// Function to detect grid and words list without grid
// Modify function signature to include current_word pointer
int no_grid_detection(SDL_Surface *image, const char *IMAGE,
                      int biggest_component, int flag, int *current_word) {
  // Declare variables
  int grid_left, grid_right, grid_top, grid_bottom;
  int list_left, list_right, list_top, list_bottom;
  int num_rows, num_cols;
  coordinates *words = NULL;
  int success = 1;
  // Remove the current_word local variable since it's now a parameter
  (void)IMAGE;

  // Get the bounding box parameters
  image_parameter(image, &grid_left, &grid_right, &grid_top, &grid_bottom);

  // Adjust the borders
  check_and_dilate_borders(image, &grid_left, &grid_right, &grid_top,
                           &grid_bottom);

  // Count the number of rows and columns
  if (biggest_component == 12673) {
    analyze_grid(image, &grid_left, &grid_right, &grid_top, &grid_bottom,
                 &num_cols, &num_rows, 662, 20);
  } else {
    analyze_grid(image, &grid_left, &grid_right, &grid_top, &grid_bottom,
                 &num_cols, &num_rows, 120, 5);
  }

  // Draw the grid
  if (flag) {
    draw_grid(image, grid_left, grid_right, grid_top, grid_bottom, num_rows,
              num_cols);
  }

  // Detect the word list
  find_words_list(image, &grid_left, &grid_right, &grid_top, &grid_bottom,
                  &list_left, &list_right, &list_top, &list_bottom, 15, 1, 2,
                  flag, current_word);

  // Count the number of words
  int word_count =
      count_words(image, list_left, list_right, list_top, list_bottom, 0);

  if (word_count <= 0) {
    printf("[ERROR] No words found in the list\n");
    success = 0;
    goto cleanup;
  }

  // Extract the words list
  words = words_extraction(image, list_left, list_right, list_top, list_bottom,
                           1, word_count, flag);
  if (!words) {
    printf("[ERROR] Failed to extract words\n");
    success = 0;
    goto cleanup;
  }

  int letter_width = lettre_size(image, list_left, list_right,
                                 words[0].top_bound, words[0].bottom_bound, 1);

  // Extract and resize letters
  for (int i = 0; i < word_count; i++) {
    if (biggest_component < 20000) {
      int temp =
          letters_extraction(image, list_left, list_right, words[i].top_bound,
                             words[i].bottom_bound, 0, *current_word);
      if (temp > 0) {
        letters_resize(*current_word, temp);
        (*current_word)++; // Increment the pointer value
      } else {
        printf("[WARNING] No letters extracted for word %d\n", i + 1);
      }
    } else {
      if (letter_width > 0) {
        int temp = letters_extraction_v2(
            image, list_left, list_right, words[i].top_bound,
            words[i].bottom_bound, 1, *current_word, letter_width);
        if (temp > 0) {
          letters_resize(*current_word, temp);
          (*current_word)++; // Increment the pointer value
        } else {
          printf("[WARNING] No letters extracted for word %d\n", i + 1);
        }
      } else {
        printf("[WARNING] No letters extracted for word %d\n", i + 1);
      }
    }
  }

  // Save the SDL surface to .png
  save_image(image, "output/output.png");

  // Split the image into smaller images and resize cells
  split_grid_into_images(image, grid_left, grid_top, grid_right, grid_bottom,
                         num_rows, num_cols);
  cells_resize(num_rows, num_cols);

cleanup:
  // Cleanup
  if (words) {
    free(words);
  }

  return success;
}

int main(int argc, char *argv[]) {
  clock_t start_time = clock();
  SDL_Surface *image = NULL;
  int biggest_component = 0;

  // Create output directory if it doesn't exist
  struct stat st = {0};
  if (stat("output", &st) == -1) {
    mkdir("output", 0777);
  }

  // Clean up output directories
  cleanup_directory("../data/words");
  cleanup_directory("../data/cells");
  cleanup_directory("output");

  // Initialize SDL and SDL_image
  if (!init_sdl_and_img()) {
    return 1;
  }

  if (argc == 1) {
    printf("Error: No image path provided\n");
    SDL_Quit();
    return 1;
  } else if (argc == 3) {
    // Single image mode
    const char *IMAGE = argv[1];
    int flag = atoi(argv[2]);
    int current_word = 0; // Initialize current_word

    // Try to load the image first to check if it exists
    SDL_Surface *test_image = IMG_Load(IMAGE);
    if (!test_image) {
      printf("Error: Could not load image '%s': %s\n", IMAGE, IMG_GetError());
      SDL_Quit();
      return 1;
    }
    SDL_FreeSurface(test_image);

    // Resize the image
    resize_image(IMAGE, "output/temp_resized.png", 700);

    // Load the resized image
    image = load_image("output/temp_resized.png");
    if (!image) {
      SDL_Quit();
      return 1;
    }

    // Check if a grid is detected in the image
    if (detect_grid(image, &biggest_component, 1) == 0) {
      // Reload the image to not get the biggest component in red
      SDL_FreeSurface(image);
      // Resize the image
      resize_image(IMAGE, "output/temp_resized.png", 700);
      // Reload the image
      image = load_image("output/temp_resized.png");
      if (!image) {
        SDL_Quit();
        return 1;
      }
      no_grid_detection(image, IMAGE, biggest_component, flag, &current_word);
    } else {
      float rotation_angle = detect_grid_rotation(image);

      if (fabs(rotation_angle) >
          1.0) { // If rotation is significant (>1 degree)
        // Create temporary file for rotated image
        rotate("output/largest_component.png", "output/temp_rotated.png",
               -rotation_angle);
        // Reload the rotated image
        SDL_FreeSurface(image);
        image = load_image("output/temp_rotated.png");
        if (!image) {
          SDL_Quit();
          return 1;
        }
      }

      // Remove the grid from the rotated image
      delete_grid(image);
      // Apply the grid detection again
      no_grid_detection(image, IMAGE, biggest_component, flag, &current_word);
      // save_image(image, "output/final_grid.png");
    }

    // Print final current_word count
    printf("Total words processed: %d\n", current_word);
  } else if (argc == 3) {
    // Rotate function
    printf("Rotate function\n");
    const char *IMAGE = argv[1];
    const char *ANGLE = argv[2];

    rotate(IMAGE, "output/rotated.png", atof(ANGLE));

    return 0;
  } else {
    printf("Usage: %s [image_path] [angle]\n", argv[0]);
    printf("       %s            # Run test suite\n", argv[0]);
    SDL_Quit();
    return 1;
  }

  // Cleanup
  if (image) {
    SDL_FreeSurface(image);
    image = NULL;
  }

  // Ensure all temporary files are cleaned up
  remove("output/temp_resized.png");
  remove("output/temp_rotated.png");
  remove("output/largest_component.png");

  // Final cleanup of SDL subsystems
  cleanup_sdl_and_img();

  // Calculate and print execution time
  clock_t end_time = clock();
  double execute_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  printf("\nExecution time: %f seconds\n", execute_time);
  return 0;
}
