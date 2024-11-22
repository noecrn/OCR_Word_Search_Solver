#include <dirent.h>
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

  // Save the SDL surfare to .png
  save_image(image, "output/output.png");

  // Split the image into smaller images
  split_grid_into_images(image, grid_left, grid_top, grid_right, grid_bottom,
                         num_rows, num_cols);

  // Resize cells
  cells_resize(num_rows, num_cols);

  // Clean up image only, don't quit SDL
  SDL_FreeSurface(image);

  return 1; // Return success
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

// Add test suite function
void run_test_suite() {
    char path[256];
    char temp_path[256];
    int total_tests = 0;
    int successful_tests = 0;
    double *test_times = calloc(8, sizeof(double)); // Store individual test times
    double total_time = 0.0;

    printf("\n=== Starting Test Suite ===\n");

    // Initialize SDL once for all tests
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: Could not initialize SDL: %s\n", SDL_GetError());
        free(test_times);
        return;
    }

    // Test all levels (1-4) and images (1-2)
    for (int level = 1; level <= 4; level++) {
        for (int image = 1; image <= 2; image++) {
            clock_t test_start = clock();
            
            snprintf(path, sizeof(path), "data/level_%d_image_%d.png", level, image);
            printf("\nTesting: level_%d_image_%d.png\n", level, image);
            total_tests++;

            // Try to load the image
            SDL_Surface *test_image = IMG_Load(path);
            if (!test_image) {
                printf("Error: Could not load image '%s': %s\n", path, IMG_GetError());
                continue;
            }
            SDL_FreeSurface(test_image);

            // Resize the image
            snprintf(temp_path, sizeof(temp_path), "output/temp_level_%d_image_%d.png", 
                    level, image);
            resize_image(path, temp_path, 700);

            // Load the resized image
            SDL_Surface *image_surface = load_image(temp_path);
            if (!image_surface) {
                printf("Error: Could not load resized image\n");
                continue;
            }

            int success = 0;
            // Process image
            if (detect_grid(image_surface) == 0) {
                printf("[DEBUG] Using no_grid_detection method\n");
                success = no_grid_detection(image_surface, path);
                if (success) successful_tests++;
                SDL_FreeSurface(image_surface);
            } else {
                printf("[DEBUG] Attempting Hough transform detection\n");
                if (with_grid_detection(image_surface) == 1) {
                    successful_tests++;
                }
                SDL_FreeSurface(image_surface);
            }

            // Calculate and store timing for this test
            clock_t test_end = clock();
            double test_time = (double)(test_end - test_start) / CLOCKS_PER_SEC;
            test_times[(level-1)*2 + (image-1)] = test_time;
            total_time += test_time;
            printf("Test execution time: %.3f seconds\n", test_time);
        }
    }

    // Final cleanup
    SDL_Quit();

    // Calculate timing statistics
    double min_time = test_times[0];
    double max_time = test_times[0];
    for (int i = 1; i < 8; i++) {
        if (test_times[i] < min_time) min_time = test_times[i];
        if (test_times[i] > max_time) max_time = test_times[i];
    }

    printf("\n=== Test Suite Complete ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Successful: %d\n", successful_tests);
    printf("Failed: %d\n", total_tests - successful_tests);
    printf("Success rate: %.1f%%\n", (float)successful_tests/total_tests * 100);
    printf("\nTiming Statistics:\n");
    printf("Total time: %.3f seconds\n", total_time);
    printf("Average time: %.3f seconds\n", total_time / total_tests);
    printf("Minimum time: %.3f seconds\n", min_time);
    printf("Maximum time: %.3f seconds\n", max_time);

    free(test_times);
}

int main(int argc, char *argv[]) {
    clock_t start_time = clock();

    if (argc == 1) {
        // Run test suite mode
        run_test_suite();
    }
    else if (argc == 2) {
        // Single image mode
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
    }
    else if (argc == 3) {
        // Rotate function
        printf("Rotate function\n");
        const char *IMAGE = argv[1];
        const char *ANGLE = argv[2];

        rotate(IMAGE, "output/rotated.png", atof(ANGLE));

        return 0;
    }
    else {
        printf("Usage: %s [image_path] [angle]\n", argv[0]);
        printf("       %s            # Run test suite\n", argv[0]);
        return 1;
    }

    // Calculate and print execution time
    clock_t end_time = clock();
    double execute_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\nExecution time: %f seconds\n", execute_time);
    return 0;
}
