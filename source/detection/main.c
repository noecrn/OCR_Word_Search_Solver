#include <stdio.h>
#include <stdlib.h>
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

    // // Draw the boundaries on the image
    SDL_Color red = {255, 0, 0, 255};
    draw_line(image, min_x, -1, red); // Draw a vertical line at min_x
    draw_line(image, max_x, -1, red); // Draw a vertical line at max_x
    draw_line(image, -1, min_y, red); // Draw a horizontal line at min_y
    draw_line(image, -1, max_y, red); // Draw a horizontal line at max_y

    draw_line(image_, min_x_, -1, red); // Draw a vertical line at min_x
    draw_line(image_, max_x_, -1, red); // Draw a vertical line at max_x
    draw_line(image_, -1, min_y_, red); // Draw a horizontal line at min_y
    draw_line(image_, -1, max_y_, red); // Draw a horizontal line at max_y

    // analyze_grid(image, min_x, max_x, min_y, max_y, &min_x, &max_x);
    // analyze_grid(image_, min_x_, max_x_, min_y_, max_y_, &min_x_, &max_x_);

    analyze_grid(image, min_x, max_x, min_y, max_y, &min_x, &max_x);
    analyze_grid(image_, min_x_, max_x_, min_y_, max_y_, &min_x_, &max_x_);

    save_image(image, "output.png");
    save_image(image_, "output_.png");

    SDL_FreeSurface(image);
    SDL_FreeSurface(image_);
    SDL_Quit();
    return 0;
}