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

// Add helper function for color check
int is_white_pixel(SDL_Surface *surface, int x, int y) {
    Uint8 r, g, b;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    SDL_GetRGB(pixels[y * surface->w + x], surface->format, &r, &g, &b);
    return (r > 200 && g > 200 && b > 200);
}

// Add line length calculation
float calculate_line_length(int x1, int y1, int x2, int y2) {
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

// Add at the top after includes
static int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void hough_transform_binary(SDL_Surface *surface, HoughLine *lines,
                            int *num_lines) {
  printf("[DEBUG] Starting improved Hough transform...\n");
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

  // Use non-maximum suppression with angle-based filtering
  const int threshold = 25;
  const int window = 5;
  float prev_theta = -1;
  
  for (int r = window; r < 2 * diagonal + 1 - window; r++) {
    for (int t = 0; t < num_thetas; t++) {
      float theta = t * M_PI / num_thetas;
      
      // Skip if too close to previous line with similar angle
      if (prev_theta != -1 && fabs(theta - prev_theta) < 0.1) {
        continue;
      }

      if (accumulator[r * num_thetas + t] > threshold) {
        // Check if it's a local maximum
        int is_max = 1;
        for (int dr = -window; dr <= window && is_max; dr++) {
          for (int dt = -window; dt <= window; dt++) {
            if (dr == 0 && dt == 0) continue;
            if (r + dr >= 0 && r + dr < 2 * diagonal + 1 &&
                t + dt >= 0 && t + dt < num_thetas) {
              if (accumulator[(r + dr) * num_thetas + (t + dt)] > 
                  accumulator[r * num_thetas + t]) {
                is_max = 0;
                break;
              }
            }
          }
        }
        
        if (is_max && *num_lines < max_lines) {
          lines[*num_lines].rho = r - diagonal;
          lines[*num_lines].theta = theta;
          prev_theta = theta;
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

  // Group lines by orientation with wider tolerance and handle rotation
  const float angle_tolerance = 0.5; // Increased to about 30 degrees
  float avg_horizontal_angle = 0;
  int horizontal_count = 0;

  for (int i = 0; i < num_lines; i++) {
    float theta = lines[i].theta;
    // Check for rotated horizontal lines in wider range
    if (fabs(theta) < angle_tolerance || fabs(theta - M_PI) < angle_tolerance ||
        fabs(theta - M_PI/4) < angle_tolerance || fabs(theta - 3*M_PI/4) < angle_tolerance) {
      h_rhos[h_count++] = lines[i].rho;
      avg_horizontal_angle += theta;
      horizontal_count++;
    }
    // Check for rotated vertical lines
    else if (fabs(theta - M_PI/2) < angle_tolerance ||
             fabs(theta - M_PI/4) < angle_tolerance || 
             fabs(theta - 3*M_PI/4) < angle_tolerance) {
      v_rhos[v_count++] = lines[i].rho;
    }
  }

  printf("[DEBUG] Found lines - H:%d V:%d (min required: 2 each)\n", h_count, v_count);

  // Calculate average rotation angle
  if (horizontal_count > 0) {
    avg_horizontal_angle /= horizontal_count;
    printf("[DEBUG] Average horizontal angle: %.2f degrees\n", 
           avg_horizontal_angle * 180 / M_PI);
  }

  // Validate grid with more flexible proportions
  if (h_count >= 2 && v_count >= 2) {
    // Sort and select outermost lines
    qsort(h_rhos, h_count, sizeof(int), compare_ints);
    qsort(v_rhos, v_count, sizeof(int), compare_ints);

    coords.top = h_rhos[0];
    coords.bottom = h_rhos[h_count - 1];
    coords.left = v_rhos[0];
    coords.right = v_rhos[v_count - 1];  // Fixed syntax error here

    // Adjust proportions check for rotated grids
    float width = abs(coords.right - coords.left);
    float height = abs(coords.bottom - coords.top);
    float ratio = width / height;

    // More flexible ratio constraints
    if (ratio < 0.4 || ratio > 2.5) {
      printf("[DEBUG] Invalid grid proportions: %.2f\n", ratio);
      coords = (GridCoords){0, 0, 0, 0};
    } else {
      printf("[DEBUG] Valid grid detected with ratio: %.2f\n", ratio);
      // Adjust coordinates for rotation if needed
      if (fabs(avg_horizontal_angle) > 0.1) {  // More than ~5 degrees
        float cos_theta = cos(avg_horizontal_angle);
        float sin_theta = sin(avg_horizontal_angle);
        // Apply rotation correction to coordinates using both sin and cos
        int center_x = (coords.left + coords.right) / 2;
        int center_y = (coords.top + coords.bottom) / 2;
        int dx, dy;
        
        dx = coords.left - center_x;
        dy = coords.top - center_y;
        coords.left = center_x + dx * cos_theta - dy * sin_theta;
        coords.top = center_y + dx * sin_theta + dy * cos_theta;

        dx = coords.right - center_x;
        dy = coords.bottom - center_y;
        coords.right = center_x + dx * cos_theta - dy * sin_theta;
        coords.bottom = center_y + dx * sin_theta + dy * cos_theta;
      }
    }
  } else {
    printf("[DEBUG] Not enough lines found for grid\n");
  }

  free(lines);
  free(h_rhos);
  free(v_rhos);

  return coords;
}