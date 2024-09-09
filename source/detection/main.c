#include <stdio.h>
#include <stdlib.h>

#include "utils/image_utils.h"
//#include "utils/image_loader.h"

int main() {
    resizeImage("data/level_1_image_1.png", "temp_resized.png", 700);
    rotate("temp_resized.png", "temp_rotated.png", 45);
    return 0;
}