#include "../include/image_processing.h"
#include "../include/image_loader.h"
#include "../include/rendering.h"
#include "../include/bounding_boxes.h"
#include "../include/line_detection.h"
#include "../include/lettre_extraction.h"

#define IMAGE "data/image-2.png"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    resize_image(IMAGE, "output/temp_resized.png", 700);
    // rotate_image("data/image-2.png", "temp_rotated.png", 90);
    SDL_Surface* image = load_image("output/temp_resized.png");

    // Appelle la fonction pour détecter la grille de lettres
    int left, right, top, bottom;
    detect_letter_grid(image, &left, &right, &top, &bottom);

    // Dilater la taille de la grille de 10 pixels
    int dilation = 15;
    left = (left - dilation > 0) ? left - dilation : 1;
    right = (right + dilation < image->w) ? right + dilation : image->w - 1;
    top = (top - dilation > 0) ? top - dilation : 1;
    bottom = (bottom + dilation < image->h) ? bottom + dilation : image->h - 1;

    // Dessine les bordures de la grille
    draw_line(image, -1, top, (SDL_Color){255, 0, 0, 255});
    draw_line(image, -1, bottom, (SDL_Color){255, 0, 0, 255});
    draw_line(image, right, -1, (SDL_Color){255, 0, 0, 255});
    draw_line(image, left, -1, (SDL_Color){255, 0, 0, 255});

    image = load_image("output/temp_resized.png");

    int num_rows, num_cols;
    analyze_grid(image, left, right, top, bottom, &num_cols, &num_rows);

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