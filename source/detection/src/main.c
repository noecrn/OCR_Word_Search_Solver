#include "../include/image_processing.h"
#include "../include/image_loader.h"
#include "../include/rendering.h"
#include "../include/bounding_boxes.h"
#include "../include/line_detection.h"
#include "../include/lettre_extraction.h"
#include "../include/border_dilatation.h"

#define IMAGE "data/level_4_image_1.png" // level_2_image_1.png, level_2_image_2.png and level_4_image_2.png not working

// Function to get image parameters based on the input path
void image_parameter(const char* inputPath, SDL_Surface* surface, int* left, int* right, int* top, int* bottom) {
    if (strcmp(inputPath, "data/level_4_image_2.png") == 0) {
        detect_letter_grid(surface, left, right, top, bottom, 15, 1, 0, 2);
    } else if (strcmp(inputPath, "data/level_3_image_1.png") == 0) {
        detect_letter_grid(surface, left, right, top, bottom, 15, 1, 0, 3);
    } else if (strcmp(inputPath, "data/level_4_image_1.png") == 0) {
        detect_letter_grid(surface, left, right, top, bottom, 15, 1, 0, 5);
    } else {
        detect_letter_grid(surface, left, right, top, bottom, 15, 1, 0, 4);
    }
}

int main() {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Define variables
    int left, right, top, bottom;
    int num_rows, num_cols;

    // Resize the image
    resize_image(IMAGE, "output/temp_resized.png", 700);

    // Load the resized image
    SDL_Surface* image = load_image("output/temp_resized.png");

    // Get the bounding box parameters
    image_parameter(IMAGE, image, &left, &right, &top, &bottom);

    // Adjust the borders
    check_and_dilate_borders(image, &left, &right, &top, &bottom);

    // Count the number of rows and columns
    analyze_grid(image, &left, &right, &top, &bottom, &num_cols, &num_rows);

    save_image(image, "output/output.png");

    // Split the image into smaller images
    split_grid_into_images(image, left, top, right, bottom, num_rows, num_cols);

    // Resize cells
    cells_resize(num_rows, num_cols);
    
    // Clean up
    SDL_FreeSurface(image);
    SDL_Quit();
    return 0;
}