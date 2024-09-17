#include "../include/image_processing.h"
#include "../include/image_loader.h"
#include "../include/rendering.h"
#include "../include/bounding_boxes.h"
#include "../include/line_detection.h"
#include "../include/lettre_extraction.h"
#include "../include/border_dilatation.h"

#define IMAGE "data/level_3_image_2.png"

void image_parameter(const char* inputPath, SDL_Surface* surface, int* left, int* right, int* top, int* bottom) {
    if (strcmp(inputPath, "data/level_4_image_2.png") == 0) {
        detect_letter_grid(surface, left, right, top, bottom, 15, 1, 0, 2);
    } else if (strcmp(inputPath, "data/level_3_image_1.png") == 0) {
        detect_letter_grid(surface, left, right, top, bottom, 15, 1, 0, 3);
    } else {
        detect_letter_grid(surface, left, right, top, bottom, 15, 1, 0, 4);
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    resize_image(IMAGE, "output/temp_resized.png", 700);
    // rotate_image("data/image-2.png", "temp_rotated.png", 90);
    SDL_Surface* image = load_image("output/temp_resized.png");

    // Appelle la fonction pour détecter la grille de lettres
    int left, right, top, bottom;
    image_parameter(IMAGE, image, &left, &right, &top, &bottom);

    check_and_dilate_borders(image, &left, &right, &top, &bottom);

    // Dessine les bordures de la grille
    draw_line(image, -1, top, (SDL_Color){255, 0, 0, 255});
    draw_line(image, -1, bottom, (SDL_Color){255, 0, 0, 255});
    draw_line(image, right, -1, (SDL_Color){255, 0, 0, 255});
    draw_line(image, left, -1, (SDL_Color){255, 0, 0, 255});

    // int num_rows, num_cols;
    // analyze_grid(image, &left, &right, &top, &bottom, &num_cols, &num_rows);
    // printf("%d %d\n", num_rows, num_cols);

    // // Recharger l'image pour dessiner la grille
    // image = load_image("temp_resized.png");

    // // Dessine la grille sur l'image
    // draw_grid(image, left, top, right, bottom, num_rows, num_cols);

    save_image(image, "output/output.png");

    // split_grid_into_images(image, left, top, right, bottom, num_rows, num_cols);

    SDL_FreeSurface(image);
    SDL_Quit();
    return 0;
}