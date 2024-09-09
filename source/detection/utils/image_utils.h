#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <SDL.h>

void rotate(const char* inputPath, const char* outputPath, double angle);
void resizeImage(const char* inputPath, const char* outputPath, int minSideSize);

#endif