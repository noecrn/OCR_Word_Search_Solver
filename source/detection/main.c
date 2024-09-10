#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils/image_utils.h"
#include "utils/image_loader.h"
#include "utils/image_draw.h"
#include "processing/bounding_boxes.h"
#include "processing/line_detection.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    resize_image("data/image-3.png", "temp_resized.png", 700);
    resize_image("data/image-1.png", "temp_resized_.png", 700);
    // rotate_image("data/image-2.png", "temp_rotated.png", 90);
    SDL_Surface* image = load_image("temp_resized.png");
    int min_x, min_y, max_x, max_y;
    SDL_Surface* image_ = load_image("temp_resized_.png");
    int min_x_, min_y_, max_x_, max_y_;

    // Appelle la fonction pour détecter la grille de lettres
    detect_letter_grid(image, &min_x, &max_x, &min_y, &max_y);
    detect_letter_grid(image_, &min_x_, &max_x_, &min_y_, &max_y_);

    // Dilater la taille de la grille de 10 pixels
    int dilation = 20;
    min_x = (min_x - dilation >= 0) ? min_x - dilation : 0;
    min_y = (min_y - dilation >= 0) ? min_y - dilation : 0;
    max_x = (max_x + dilation < image->w) ? max_x + dilation : image->w - 1;
    max_y = (max_y + dilation < image->h) ? max_y + dilation : image->h - 1;

    min_x_ = (min_x_ - dilation >= 0) ? min_x_ - dilation : 0;
    min_y_ = (min_y_ - dilation >= 0) ? min_y_ - dilation : 0;
    max_x_ = (max_x_ + dilation < image_->w) ? max_x_ + dilation : image_->w - 1;
    max_y_ = (max_y_ + dilation < image_->h) ? max_y_ + dilation : image_->h - 1;

    int num_rows, num_cols;
    analyze_grid(image, min_x, max_x, min_y, max_y, &num_cols, &num_rows);
    int num_rows_, num_cols_;
    analyze_grid(image_, min_x_, max_x_, min_y_, max_y_, &num_cols_, &num_rows_);

    printf("Grid size: %dx%d\n", num_rows, num_cols);
    printf("Grid size: %dx%d\n", num_rows_, num_cols_);

    printf("Grid bounds: (%d, %d) - (%d, %d)\n", min_x, min_y, max_x, max_y);
    printf("Grid bounds: (%d, %d) - (%d, %d)\n", min_x_, min_y_, max_x_, max_y_);

    // Recharger l'image pour dessiner la grille
    image = load_image("temp_resized.png");
    image_ = load_image("temp_resized_.png");

    // Dessine la grille sur l'image
    draw_grid(image, min_x, max_x, min_y, max_y, num_rows, num_cols);
    draw_grid(image_, min_x_, max_x_, min_y_, max_y_, num_rows_, num_cols_);

    save_image(image, "output.png");
    save_image(image_, "output_.png");

    SDL_FreeSurface(image);
    SDL_FreeSurface(image_);
    SDL_Quit();
    return 0;
}