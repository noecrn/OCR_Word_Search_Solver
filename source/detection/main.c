#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils/image_utils.h"
#include "utils/image_loader.h"
#include "utils/image_draw.h"
#include "processing/bounding_boxes.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    // resize_image("data/image-2.png", "temp_resized.png", 700);
    // rotate_image("data/image-2.png", "temp_rotated.png", 90);
    SDL_Surface* image = load_image("data/image-1.png");
    int min_x, min_y, max_x, max_y;

    // Appelle la fonction pour détecter la grille de lettres
    detect_letter_grid(image, &min_x, &max_x, &min_y, &max_y);

    // Draw the boundaries on the image
    SDL_Color red = {255, 0, 0, 255};
    draw_line(image, min_x, -1, red); // Draw a vertical line at min_x
    draw_line(image, max_x, -1, red); // Draw a vertical line at max_x
    draw_line(image, -1, min_y, red); // Draw a horizontal line at min_y
    draw_line(image, -1, max_y, red); // Draw a horizontal line at max_y

    save_image(image, "output.png");

    SDL_FreeSurface(image);
    SDL_Quit();
    return 0;
}