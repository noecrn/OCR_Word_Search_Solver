#include "../../include/bounding_boxes.h"
#include <SDL.h>
#include <errno.h>

#define BLACK_TOLERANCE 1
#define DILATION 10

// Helper function to safely get pixel at position
static inline Uint32 get_pixel(SDL_Surface *surface, int x, int y,
                               int pitch_div4) {
  return ((Uint32 *)surface->pixels)[y * pitch_div4 + x];
}

// Optimized black pixel check for a line
static int has_black_pixels_on_line(SDL_Surface *surface, int y, int left,
                                    int right, int pitch_div4) {
  Uint8 r, g, b, a;
  for (int x = left; x <= right; x++) {
    Uint32 pixel = get_pixel(surface, x, y, pitch_div4);
    SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
    if (is_black_pixel(r, g, b, BLACK_TOLERANCE))
      return 1;
  }
  return 0;
}

// Optimized black pixel check for a column
static int has_black_pixels_on_column(SDL_Surface *surface, int x, int top,
                                      int bottom, int pitch_div4) {
  Uint8 r, g, b, a;
  for (int y = top; y <= bottom; y++) {
    Uint32 pixel = get_pixel(surface, x, y, pitch_div4);
    SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
    if (is_black_pixel(r, g, b, BLACK_TOLERANCE))
      return 1;
  }
  return 0;
}

// Main function with error handling and optimization
int check_and_dilate_borders(SDL_Surface *surface, int *left, int *right,
                             int *top, int *bottom) {
  // Parameter validation
  if (!surface || !left || !right || !top || !bottom)
    return -EINVAL;

  // Cache surface properties
  const int width = surface->w;
  const int height = surface->h;
  const int pitch_div4 = surface->pitch / 4;

  // Validate boundaries
  if (*left < 0 || *right >= width || *top < 0 || *bottom >= height ||
      *left > *right || *top > *bottom)
    return -ERANGE;

  // Check and dilate top border
  if (has_black_pixels_on_line(surface, *top, *left, *right, pitch_div4))
    *top = (*top - DILATION > 0) ? *top - DILATION : 1;

  // Check and dilate bottom border
  if (has_black_pixels_on_line(surface, *bottom, *left, *right, pitch_div4))
    *bottom = (*bottom + DILATION < height) ? *bottom + DILATION : height - 1;

  // Check and dilate left border
  if (has_black_pixels_on_column(surface, *left, *top, *bottom, pitch_div4))
    *left = (*left - DILATION > 0) ? *left - DILATION : 1;

  // Check and dilate right border
  if (has_black_pixels_on_column(surface, *right, *top, *bottom, pitch_div4))
    *right = (*right + DILATION < width) ? *right + DILATION : width - 1;

  return 0;
}