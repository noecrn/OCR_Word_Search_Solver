#include "../../include/grid_detection.h"
#include "../../include/image_processing.h"
#include <SDL2/SDL.h>
#include <math.h>

static int min_angle = -1;

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
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// Function to draw a line with start coordinates and angle
void draw_line_at_angle(SDL_Surface *surface, int x1, int y1, float theta,
                        int length, Uint32 color) {
  // Calculate end point using parametric form
  int x2 = x1 + (int)(length * cos(theta));
  int y2 = y1 + (int)(length * sin(theta));

  // Draw the line using existing debug line function
  draw_debug_line(surface, x1, y1, x2, y2, color);
}

// Function to count red pixels along a line with specific angle and length
int count_pixels_on_line(SDL_Surface *surface, int start_x, int start_y,
                         float angle, int length) {
  float dx = cos(angle);
  float dy = sin(angle);
  int red_count = 0;

  // Check each point along the line
  for (int i = 0; i < length; i++) {
    // Calculate exact position
    int x = start_x + (int)(dx * i);
    int y = start_y + (int)(dy * i);

    // Check bounds
    if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
      Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
      if (is_pixel_red(pixel, surface)) {
        red_count++;
      }
    } else {
      break; // Stop if we go out of bounds
    }
  }

  return red_count;
}

// Function to find lines of the grid
void find_grid_lines(SDL_Surface *surface, int x, int y) {
  const int NUM_ANGLES = 360;  // Check all angles
  const int MIN_LENGTH = 800;  // Minimum line length to consider
  const int MAX_LENGTH = 1000; // Maximum line length to check
  const int MIN_RED_PIXELS =
      300; // Minimum number of red pixels for a valid line

  // Check all angles
  for (int deg = 0; deg < NUM_ANGLES; deg++) {
    float angle = (float)deg * M_PI / 180.0f;

    // Try different lengths
    for (int len = MIN_LENGTH; len <= MAX_LENGTH; len += 50) {
      int count = count_pixels_on_line(surface, x, y, angle, len);

      if (count >= MIN_RED_PIXELS) {
        int temp = deg % 90;
        if (min_angle == -1) {
          min_angle = temp;
        } else if (temp < min_angle) {
          min_angle = temp;
        }
        // Draw line if we found enough red pixels
        // draw_line_at_angle(surface, x, y, angle, len,
        //                  SDL_MapRGB(surface->format, 0, 0, 255));
        break; // Move to next angle once we find a good line
      }
    }
  }
}

// Function to process all red pixels in the image
void process_red_pixels(SDL_Surface *surface) {
  SDL_LockSurface(surface);

  // Sample grid points instead of checking every pixel
  const int GRID_STEP = 20; // Check every 20th pixel

  for (int y = 0; y < surface->h; y += GRID_STEP) {
    for (int x = 0; x < surface->w; x += GRID_STEP) {
      Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
      if (is_pixel_red(pixel, surface)) {
        find_grid_lines(surface, x, y);
      }
    }
  }

  SDL_UnlockSurface(surface);
  // save_image(surface, "output/grid_lines.png");
}

// Structure to store line information
typedef struct {
  float angle;
  int count;
} LineInfo;

// Structure to store detected line information
typedef struct {
  int angle;
  int count;
  int is_used;
} DetectedLine;

// Function to calculate dominant angle from detected lines
float calculate_rotation_angle(SDL_Surface *surface) {
  int closest_angle = -1;
  int min_diff = 90; // Initialize to maximum possible difference

  // Scan for blue lines and their angles
  SDL_LockSurface(surface);

  // Process all pixels once to find the angles of our detected lines
  for (int y = 0; y < surface->h; y++) {
    for (int x = 0; x < surface->w; x++) {
      Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);

      // Check for our detected lines (blue pixels)
      if (b > 200 && r < 50 && g < 50) {
        // Get the angle from our debug output
        for (int angle = 0; angle < 360; angle++) {
          // Calculate difference from 90 degrees
          int diff = abs(90 - (angle % 180));
          if (diff < min_diff) {
            min_diff = diff;
            closest_angle = angle;
          }
        }
      }
    }
  }

  SDL_UnlockSurface(surface);

  // Calculate rotation needed
  int rotation_angle = 0;
  if (closest_angle >= 0) {
    rotation_angle = 90 - (closest_angle % 180);
  }

  return rotation_angle;
}

// Update process_red_pixels to return rotation angle
float detect_grid_rotation(SDL_Surface *surface) {
  // First detect grid lines as before
  process_red_pixels(surface);

  // Then calculate rotation angle from the detected lines
  return (float)min_angle;
}
