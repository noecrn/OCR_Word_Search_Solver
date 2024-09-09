#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <SDL.h>

Uint32** loadImageToMatrix(const char* filename, int* width, int* height);

#endif