#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <SDL2/SDL.h>

void rotate_image(const char* inputPath, const char* outputPath, double angle);
void resize_image(const char* inputPath, const char* outputPath, int minSideSize);
void save_image(SDL_Surface* surface, const char* outputPath);

#endif