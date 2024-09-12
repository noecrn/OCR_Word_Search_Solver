#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/image_processing.h"
#include "../include/image_loader.h"
#include "../include/rendering.h"
#include "../include/bounding_boxes.h"
#include "../include/line_detection.h"
#include "../include/lettre_extraction.h"

#define IMAGE "data/image-1.png"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    resize_image(IMAGE, "temp_resized.png", 700);
    // rotate_image("data/image-2.png", "temp_rotated.png", 90);
    SDL_Surface* image = load_image("temp_resized.png");
    int min_x, min_y, max_x, max_y;

    // Appelle la fonction pour détecter la grille de lettres
    detect_letter_grid(image, &min_x, &max_x, &min_y, &max_y);

    // Dilater la taille de la grille de 10 pixels
    int dilation = 20;
    min_x = (min_x - dilation >= 0) ? min_x - dilation : 0;
    min_y = (min_y - dilation >= 0) ? min_y - dilation : 0;
    max_x = (max_x + dilation < image->w) ? max_x + dilation : image->w - 1;
    max_y = (max_y + dilation < image->h) ? max_y + dilation : image->h - 1;

    int num_rows, num_cols;
    analyze_grid(image, min_x, max_x, min_y, max_y, &num_cols, &num_rows);

    printf("Grid size: %dx%d\n", num_rows, num_cols);

    printf("Grid bounds: (%d, %d) - (%d, %d)\n", min_x, min_y, max_x, max_y);

    // Recharger l'image pour dessiner la grille
    image = load_image("temp_resized.png");

    // Dessine la grille sur l'image
    // draw_grid(image, min_x, max_x, min_y, max_y, num_rows, num_cols);

    save_image(image, "output.png");

    split_grid_into_images(image, min_x, max_x, min_y, max_y, num_rows, num_cols);

    SDL_FreeSurface(image);
    SDL_Quit();
    return 0;
}