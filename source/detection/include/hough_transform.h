#ifndef HOUGH_TRANSFORM_H
#define HOUGH_TRANSFORM_H

#include <SDL2/SDL.h>

typedef struct {
  int rho;
  float theta;
} HoughLine;

typedef struct {
  int left;
  int right;
  int top;
  int bottom;
} GridCoords;

void hough_transform_binary(SDL_Surface *image, HoughLine *lines,
                            int *num_lines);
GridCoords detect_grid_coords(SDL_Surface *image);

#endif