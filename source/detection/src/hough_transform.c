#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

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

void hough_transform_binary(SDL_Surface *surface, HoughLine *lines,
                            int max_lines, int *num_lines) {
  SDL_LockSurface(surface);
  Uint32 *pixels = (Uint32 *)surface->pixels;

  const int width = surface->w;
  const int height = surface->h;
  const int diagonal = (int)sqrt((double)(width * width + height * height));
  const int num_thetas = 180;
  const size_t acc_size = (size_t)(2 * diagonal + 1) * num_thetas;

  // Check for potential overflow
  if (acc_size > SIZE_MAX / sizeof(int)) {
    SDL_UnlockSurface(surface);
    *num_lines = 0;
    return;
  }

  int *accumulator = calloc(acc_size, sizeof(int));
  if (!accumulator) {
    SDL_UnlockSurface(surface);
    *num_lines = 0;
    return;
  }

  // Process pixels
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Uint8 r, g, b;
      SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);

      if (r == 0) {
        for (int t = 0; t < num_thetas; t++) {
          float theta = t * M_PI / num_thetas;
          int rho = (int)(x * cos(theta) + y * sin(theta)) + diagonal;
          if (rho >= 0 && rho < 2 * diagonal + 1) {
            size_t idx = (size_t)rho * num_thetas + t;
            if (idx < acc_size) {
              accumulator[idx]++;
            }
          }
        }
      }
    }
  }

  // Find peaks
  *num_lines = 0;
  const int threshold = 50;
  for (int r = 0; r < 2 * diagonal + 1 && *num_lines < max_lines; r++) {
    for (int t = 0; t < num_thetas && *num_lines < max_lines; t++) {
      size_t idx = (size_t)r * num_thetas + t;
      if (idx < acc_size && accumulator[idx] > threshold) {
        lines[*num_lines].rho = r - diagonal;
        lines[*num_lines].theta = t * M_PI / num_thetas;
        (*num_lines)++;
      }
    }
  }

  free(accumulator);
  SDL_UnlockSurface(surface);
}

GridCoords detect_grid_coords(SDL_Surface *image) {
  GridCoords coords = {0, 0, 0, 0};
  const int MAX_LINES = 1000;
  HoughLine *lines = NULL;
  int *h_rhos = NULL;
  int *v_rhos = NULL;
  int num_lines = 0;

  if (!image) {
    return coords;
  }

  lines = calloc(MAX_LINES, sizeof(HoughLine));
  if (!lines) {
    return coords;
  }

  hough_transform_binary(image, lines, MAX_LINES, &num_lines);

  if (num_lines == 0) {
    free(lines);
    return coords;
  }

  h_rhos = calloc(MAX_LINES, sizeof(int));
  v_rhos = calloc(MAX_LINES, sizeof(int));

  if (!h_rhos || !v_rhos) {
    free(lines);
    free(h_rhos); // Safe to free NULL
    free(v_rhos); // Safe to free NULL
    return coords;
  }

  int h_count = 0, v_count = 0;

  // Separate horizontal and vertical lines
  for (int i = 0; i < num_lines && i < MAX_LINES; i++) {
    float theta = lines[i].theta;
    if (fabs(theta) < 0.1 || fabs(theta - M_PI) < 0.1) {
      h_rhos[h_count++] = lines[i].rho;
    } else if (fabs(theta - M_PI / 2) < 0.1) {
      v_rhos[v_count++] = lines[i].rho;
    }
  }

  // Process coordinates only if we have enough lines
  if (h_count >= 2 && v_count >= 2) {
    // Tri des rhos horizontaux
    for (int i = 0; i < h_count - 1; i++) {
      for (int j = 0; j < h_count - i - 1; j++) {
        if (h_rhos[j] > h_rhos[j + 1]) {
          int temp = h_rhos[j];
          h_rhos[j] = h_rhos[j + 1];
          h_rhos[j + 1] = temp;
        }
      }
    }

    // Tri des rhos verticaux
    for (int i = 0; i < v_count - 1; i++) {
      for (int j = 0; j < v_count - i - 1; j++) {
        if (v_rhos[j] > v_rhos[j + 1]) {
          int temp = v_rhos[j];
          v_rhos[j] = v_rhos[j + 1];
          v_rhos[j + 1] = temp;
        }
      }
    }

    coords.top = h_rhos[0];
    coords.bottom = h_rhos[h_count - 1];
    coords.left = v_rhos[0];
    coords.right = v_rhos[v_count - 1];
  }

  // Cleanup
  free(h_rhos);
  free(v_rhos);
  free(lines);

  return coords;
}
