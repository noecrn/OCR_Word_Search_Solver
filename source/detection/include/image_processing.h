#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>

void rotate(const char *inputPath, const char *outputPath, double angle);
void resize_image(const char *inputPath, const char *outputPath,
                  int targetSize);
void resize_image_square(const char *inputPath, const char *outputPath,
                         int targetSize);
void save_image(SDL_Surface *surface, const char *outputPath);
void cells_resize(int num_rows, int num_cols);
void letters_resize(int num_words, int num_letters);
void display_image(SDL_Surface *image);
void draw_debug_line(SDL_Surface *surface, int x1, int y1, int x2, int y2,
                     Uint32 color);

#endif