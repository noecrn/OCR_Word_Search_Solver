#include <SDL2/SDL.h>
#include <math.h>

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
                            int *num_lines) {
  printf("[DEBUG] Starting Hough transform...\n");
  SDL_LockSurface(surface);
  Uint32 *pixels = (Uint32 *)surface->pixels;

  const int width = surface->w;
  const int height = surface->h;
  // Use double for intermediate calculation to prevent overflow
  const int diagonal = (int)ceil(sqrt((double)width * width + (double)height * height));
  const int num_thetas = 180;
  const int max_lines = 1000; // Maximum number of lines to detect

  // Allocate accumulator with proper size
  const size_t acc_size = (2 * diagonal + 1) * num_thetas;
  int *accumulator = calloc(acc_size, sizeof(int));
  if (!accumulator) {
    printf("[DEBUG] Failed to allocate accumulator\n");
    SDL_UnlockSurface(surface);
    *num_lines = 0;
    return;
  }
  printf("[DEBUG] Accumulator size: %zu\n", acc_size);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Uint8 r, g, b;
      SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);

      if (r == 0) {
        for (int t = 0; t < num_thetas; t++) {
          float theta = t * M_PI / num_thetas;
          int rho = (int)round(x * cos(theta) + y * sin(theta));
          rho += diagonal; // Shift to ensure positive index
          
          // Bounds check
          if (rho >= 0 && rho < (2 * diagonal + 1)) {
            accumulator[rho * num_thetas + t]++;
          }
        }
      }
    }
  }

  // Find peaks with much lower threshold for vertical lines
  const int threshold = 20;  // Lowered further to catch more potential lines
  *num_lines = 0;
  
  // Non-maximum suppression window size
  const int window = 5;
  
  // Find local maxima in accumulator
  for (int r = window; r < 2 * diagonal + 1 - window; r++) {
    for (int t = window; t < num_thetas - window; t++) {
      if (accumulator[r * num_thetas + t] > threshold) {
        // Check if it's a local maximum
        int is_max = 1;
        for (int dr = -window; dr <= window && is_max; dr++) {
          for (int dt = -window; dt <= window; dt++) {
            if (dr == 0 && dt == 0) continue;
            if (accumulator[(r + dr) * num_thetas + (t + dt)] > 
                accumulator[r * num_thetas + t]) {
              is_max = 0;
              break;
            }
          }
        }
        
        if (is_max && *num_lines < max_lines) {
          lines[*num_lines].rho = r - diagonal;
          lines[*num_lines].theta = t * M_PI / num_thetas;
          (*num_lines)++;
        }
      }
    }
  }

  printf("[DEBUG] Found %d lines\n", *num_lines);
  SDL_UnlockSurface(surface);
  free(accumulator);
}

GridCoords detect_grid_coords(SDL_Surface *image) {
  printf("[DEBUG] Starting grid coordinates detection...\n");
  GridCoords coords = {0, 0, 0, 0};
  HoughLine *lines = malloc(1000 * sizeof(HoughLine));
  if (!lines) {
    return coords;
  }
  int num_lines = 0;

  hough_transform_binary(image, lines, &num_lines);

  // Arrays for storing rho values
  int *h_rhos = malloc(num_lines * sizeof(int));
  int *v_rhos = malloc(num_lines * sizeof(int));
  if (!h_rhos || !v_rhos) {
    free(lines);
    free(h_rhos);
    free(v_rhos);
    return coords;
  }
  int h_count = 0, v_count = 0;

  // Separate horizontal and vertical lines with wider tolerance
  for (int i = 0; i < num_lines; i++) {
    float theta = lines[i].theta;
    // Horizontal lines: 0° or 180° ±15°
    if (fabs(theta) < 0.26 || fabs(theta - M_PI) < 0.26) {
      h_rhos[h_count++] = lines[i].rho;
    }
    // Vertical lines: 90° ±15°
    else if (fabs(theta - M_PI/2) < 0.26) {
      v_rhos[v_count++] = lines[i].rho;
    }
  }

  printf("[DEBUG] Line detection angles: H:±15°, V:90°±15°\n");
  printf("[DEBUG] Horizontal lines: %d, Vertical lines: %d\n", h_count, v_count);

  // Sort rhos
  for (int i = 0; i < h_count - 1; i++) {
    for (int j = 0; j < h_count - i - 1; j++) {
      if (h_rhos[j] > h_rhos[j + 1]) {
        int temp = h_rhos[j];
        h_rhos[j] = h_rhos[j + 1];
        h_rhos[j + 1] = temp;
      }
    }
  }

  for (int i = 0; i < v_count - 1; i++) {
    for (int j = 0; j < v_count - i - 1; j++) {
      if (v_rhos[j] > v_rhos[j + 1]) {
        int temp = v_rhos[j];
        v_rhos[j] = v_rhos[j + 1];
        v_rhos[j + 1] = temp;
      }
    }
  }

  // Get grid boundaries, require fewer lines for valid grid
  if (h_count >= 1 && v_count >= 1) {  // Changed from 2 to 1
    coords.top = h_rhos[0];
    coords.bottom = h_rhos[h_count - 1];
    coords.left = v_rhos[0];
    coords.right = v_rhos[v_count - 1];
    printf("[DEBUG] Grid bounds: T:%d B:%d L:%d R:%d\n", 
           coords.top, coords.bottom, coords.left, coords.right);
  } else {
    printf("[DEBUG] Not enough lines found for grid\n");
  }

  free(lines);
  free(h_rhos);
  free(v_rhos);

  return coords;
}