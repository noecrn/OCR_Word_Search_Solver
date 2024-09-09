#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils/image_utils.h"
#include "utils/image_loader.h"
#include "processing/bounding_boxes.h"

int main() {
    // resize_image("data/image-2.png", "temp_resized.png", 700);
    rotate_image("data/image-2.png", "temp_rotated.png", 90);
    SDL_Surface* image = load_image("temp_rotated.png");
    detect_boundaries(image);

    return 0;
}