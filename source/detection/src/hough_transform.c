#include "../include/grid_detection.h"
#include "../include/image_processing.h"
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
// static int compare_ints(const void* a, const void* b) {
//     return (*(int*)a - *(int*)b);
// }

// Function to calculate points along a line using Bresenham's algorithm
// int get_line_points(int x1, int y1, int x2, int y2, int points[][2], int max_points) {
//     int dx = abs(x2 - x1);
//     int dy = abs(y2 - y1);
//     int sx = (x1 < x2) ? 1 : -1;
//     int sy = (y1 < y2) ? 1 : -1;
//     int err = dx - dy;
//     int point_count = 0;
    
//     int current_x = x1;
//     int current_y = y1;
    
//     while (point_count < max_points) {
//         points[point_count][0] = current_x;
//         points[point_count][1] = current_y;
//         point_count++;
        
//         if (current_x == x2 && current_y == y2) break;
        
//         int e2 = 2 * err;
//         if (e2 > -dy) { err -= dy; current_x += sx; }
//         if (e2 < dx) { err += dx; current_y += sy; }
//     }
    
//     return point_count;
// }

// Helper function to check if a point is near a corner
// int is_corner_point(SDL_Surface *surface, int x, int y, int radius) {
//     if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
//         return 0;
//     }

//     int black_count = 0;
//     int total_points = 0;

//     for (int dy = -radius; dy <= radius; dy++) {
//         for (int dx = -radius; dx <= radius; dx++) {
//             int nx = x + dx;
//             int ny = y + dy;
//             if (nx >= 0 && nx < surface->w && ny >= 0 && ny < surface->h) {
//                 total_points++;
//                 Uint32 pixel = ((Uint32*)surface->pixels)[ny * surface->w + nx];
//                 if (is_pixel_black(pixel, surface)) {
//                     black_count++;
//                 }
//             }
//         }
//     }

//     // A corner should have roughly 25% black pixels in its neighborhood
//     float black_ratio = (float)black_count / total_points;
//     return black_ratio > 0.2 && black_ratio < 0.3;
// }

// Function to validate if a line represents a grid edge
// int validate_line(SDL_Surface *surface, int x1, int y1, int x2, int y2) {
//     // Constants for validation
//     const int CORNER_RADIUS = 5;
//     const float MIN_CORNER_RATIO = 0.7f;
//     const int MIN_EDGE_PIXELS = 20;
    
//     // Get points along the line
//     int points[1000][2];
//     int num_points = get_line_points(x1, y1, x2, y2, points, 1000);
    
//     // Check if both endpoints are near corners
//     int has_start_corner = is_corner_point(surface, x1, y1, CORNER_RADIUS);
//     int has_end_corner = is_corner_point(surface, x2, y2, CORNER_RADIUS);
    
//     if (!has_start_corner || !has_end_corner) {
//         return 0;
//     }

//     // Count black pixels along the edge
//     int edge_pixels = 0;
//     for (int i = 0; i < num_points; i++) {
//         int x = points[i][0];
//         int y = points[i][1];
//         if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
//             Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
//             if (is_pixel_black(pixel, surface)) {
//                 edge_pixels++;
//             }
//         }
//     }

//     // Calculate edge pixel ratio
//     float edge_ratio = (float)edge_pixels / num_points;
    
//     // Line must have significant number of edge pixels and connect two corners
//     return edge_pixels > MIN_EDGE_PIXELS && edge_ratio > MIN_CORNER_RATIO;
// }

// Function to draw a line with start coordinates and angle
void draw_line_at_angle(SDL_Surface *surface, int x1, int y1, float theta, int length, Uint32 color) {
    // Calculate end point using parametric form
    int x2 = x1 + (int)(length * cos(theta));
    int y2 = y1 + (int)(length * sin(theta));
    
    // Draw the line using existing debug line function
    draw_debug_line(surface, x1, y1, x2, y2, color);
}

// Function to count red pixels along a line with specific angle and length
int count_pixels_on_line(SDL_Surface *surface, int start_x, int start_y, float angle, int length) {
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
            Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
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
    const int MIN_LENGTH = 800;   // Minimum line length to consider
    const int MAX_LENGTH = 1000; // Maximum line length to check
    const int MIN_RED_PIXELS = 300; // Minimum number of red pixels for a valid line
    
    // Check all angles
    for (int deg = 0; deg < NUM_ANGLES; deg++) {
        float angle = (float)deg * M_PI / 180.0f;
        
        // Try different lengths
        for (int len = MIN_LENGTH; len <= MAX_LENGTH; len += 50) {
            int count = count_pixels_on_line(surface, x, y, angle, len);
            
            if (count >= MIN_RED_PIXELS) {
                // Draw line if we found enough red pixels
                printf("Found line at angle %d with %d red pixels (length %d)\n", 
                       deg, count, len);
                draw_line_at_angle(surface, x, y, angle, len, 
                                 SDL_MapRGB(surface->format, 0, 0, 255));
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
            Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
            if (is_pixel_red(pixel, surface)) {
                find_grid_lines(surface, x, y);
            }
        }
    }
    
    SDL_UnlockSurface(surface);
    save_image(surface, "output/grid_lines.png");
}

// void hough_transform_binary(SDL_Surface *surface, HoughLine *lines,
//                             int *num_lines) {
//   printf("[DEBUG] Starting improved Hough transform...\n");
//   SDL_LockSurface(surface);
//   Uint32 *pixels = (Uint32 *)surface->pixels;

//   const int width = surface->w;
//   const int height = surface->h;
//   // Use double for intermediate calculation to prevent overflow
//   const int diagonal = (int)ceil(sqrt((double)width * width + (double)height * height));
//   const int num_thetas = 180;
//   const int max_lines = 1000; // Maximum number of lines to detect

//   // Allocate accumulator with proper size
//   const size_t acc_size = (2 * diagonal + 1) * num_thetas;
//   int *accumulator = calloc(acc_size, sizeof(int));
//   if (!accumulator) {
//     printf("[DEBUG] Failed to allocate accumulator\n");
//     SDL_UnlockSurface(surface);
//     *num_lines = 0;
//     return;
//   }
//   printf("[DEBUG] Accumulator size: %zu\n", acc_size);

//   for (int y = 0; y < height; y++) {
//     for (int x = 0; x < width; x++) {
//       Uint8 r, g, b;
//       SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);

//       if (r == 0) {
//         for (int t = 0; t < num_thetas; t++) {
//           float theta = t * M_PI / num_thetas;
//           int rho = (int)round(x * cos(theta) + y * sin(theta));
//           rho += diagonal; // Shift to ensure positive index
          
//           // Bounds check
//           if (rho >= 0 && rho < (2 * diagonal + 1)) {
//             accumulator[rho * num_thetas + t]++;
//           }
//         }
//       }
//     }
//   }

//   // Use non-maximum suppression with angle-based filtering
//   const int threshold = 25;
//   const int window = 5;
//   float prev_theta = -1;
  
//   for (int r = window; r < 2 * diagonal + 1 - window; r++) {
//     for (int t = 0; t < num_thetas; t++) {
//       float theta = t * M_PI / num_thetas;
      
//       // Skip if too close to previous line with similar angle
//       if (prev_theta != -1 && fabs(theta - prev_theta) < 0.1) {
//         continue;
//       }

//       if (accumulator[r * num_thetas + t] > threshold) {
//         // Check if it's a local maximum
//         int is_max = 1;
//         for (int dr = -window; dr <= window && is_max; dr++) {
//           for (int dt = -window; dt <= window; dt++) {
//             if (dr == 0 && dt == 0) continue;
//             if (r + dr >= 0 && r + dr < 2 * diagonal + 1 &&
//                 t + dt >= 0 && t + dt < num_thetas) {
//               if (accumulator[(r + dr) * num_thetas + (t + dt)] > 
//                   accumulator[r * num_thetas + t]) {
//                 is_max = 0;
//                 break;
//               }
//             }
//           }
//         }
        
//         if (is_max && *num_lines < max_lines) {
//           lines[*num_lines].rho = r - diagonal;
//           lines[*num_lines].theta = theta;
//           prev_theta = theta;
//           (*num_lines)++;
//         }
//       }
//     }
//   }

//   printf("[DEBUG] Found %d lines\n", *num_lines);

//   int validated_count = 0;
//   HoughLine *temp_lines = malloc(max_lines * sizeof(HoughLine));
//   if (!temp_lines) {
//       free(accumulator);
//       SDL_UnlockSurface(surface);
//       *num_lines = 0;
//       return;
//   }

//   // First pass: collect all potential lines
//   int potential_count = 0;
//   for (int i = 0; i < *num_lines; i++) {
//       temp_lines[potential_count] = lines[i];
//       potential_count++;
//   }

//   // Second pass: validate lines
//   for (int i = 0; i < potential_count; i++) {
//       float theta = temp_lines[i].theta;
//       int rho = temp_lines[i].rho;
      
//       // Calculate endpoints
//       int x1 = 0;
//       int y1 = (int)((rho - x1 * cos(theta)) / sin(theta));
//       int x2 = surface->w - 1;
//       int y2 = (int)((rho - x2 * cos(theta)) / sin(theta));

//       // Draw line for debugging
//       draw_debug_line(surface, x1, y1, x2, y2, 0xFF0000FF);
//       save_image(surface, "output/----------.png");
      
//       if (validate_line(surface, x1, y1, x2, y2)) {
//           lines[validated_count] = temp_lines[i];
//           validated_count++;
//       }
//   }

//   *num_lines = validated_count;
//   free(temp_lines);
//   SDL_UnlockSurface(surface);
//   free(accumulator);
// }

// GridCoords detect_grid_coords(SDL_Surface *image) {
//   printf("[DEBUG] Starting grid coordinates detection...\n");
//   GridCoords coords = {0, 0, 0, 0};
//   HoughLine *lines = malloc(1000 * sizeof(HoughLine));
//   if (!lines) {
//     return coords;
//   }
//   int num_lines = 0;

//   hough_transform_binary(image, lines, &num_lines);

//   // Arrays for storing rho values
//   int *h_rhos = malloc(num_lines * sizeof(int));
//   int *v_rhos = malloc(num_lines * sizeof(int));
//   if (!h_rhos || !v_rhos) {
//     free(lines);
//     free(h_rhos);
//     free(v_rhos);
//     return coords;
//   }
//   int h_count = 0, v_count = 0;

//   // Group lines by orientation with wider tolerance and handle rotation
//   const float angle_tolerance = 0.5; // Increased to about 30 degrees
//   float avg_horizontal_angle = 0;
//   int horizontal_count = 0;

//   for (int i = 0; i < num_lines; i++) {
//     float theta = lines[i].theta;
//     // Check for rotated horizontal lines in wider range
//     if (fabs(theta) < angle_tolerance || fabs(theta - M_PI) < angle_tolerance ||
//         fabs(theta - M_PI/4) < angle_tolerance || fabs(theta - 3*M_PI/4) < angle_tolerance) {
//       h_rhos[h_count++] = lines[i].rho;
//       avg_horizontal_angle += theta;
//       horizontal_count++;
//     }
//     // Check for rotated vertical lines
//     else if (fabs(theta - M_PI/2) < angle_tolerance ||
//              fabs(theta - M_PI/4) < angle_tolerance || 
//              fabs(theta - 3*M_PI/4) < angle_tolerance) {
//       v_rhos[v_count++] = lines[i].rho;
//     }
//   }

//   printf("[DEBUG] Found lines - H:%d V:%d (min required: 2 each)\n", h_count, v_count);

//   // Calculate average rotation angle
//   if (horizontal_count > 0) {
//     avg_horizontal_angle /= horizontal_count;
//     printf("[DEBUG] Average horizontal angle: %.2f degrees\n", 
//            avg_horizontal_angle * 180 / M_PI);
//   }

//   // Validate grid with more flexible proportions
//   if (h_count >= 2 && v_count >= 2) {
//     // Sort and select outermost lines
//     qsort(h_rhos, h_count, sizeof(int), compare_ints);
//     qsort(v_rhos, v_count, sizeof(int), compare_ints);

//     coords.top = h_rhos[0];
//     coords.bottom = h_rhos[h_count - 1];
//     coords.left = v_rhos[0];
//     coords.right = v_rhos[v_count - 1];  // Fixed syntax error here

//     // Adjust proportions check for rotated grids
//     float width = abs(coords.right - coords.left);
//     float height = abs(coords.bottom - coords.top);
//     float ratio = width / height;

//     // More flexible ratio constraints
//     if (ratio < 0.4 || ratio > 2.5) {
//       printf("[DEBUG] Invalid grid proportions: %.2f\n", ratio);
//       coords = (GridCoords){0, 0, 0, 0};
//     } else {
//       printf("[DEBUG] Valid grid detected with ratio: %.2f\n", ratio);
//       // Adjust coordinates for rotation if needed
//       if (fabs(avg_horizontal_angle) > 0.1) {  // More than ~5 degrees
//         float cos_theta = cos(avg_horizontal_angle);
//         float sin_theta = sin(avg_horizontal_angle);
//         // Apply rotation correction to coordinates using both sin and cos
//         int center_x = (coords.left + coords.right) / 2;
//         int center_y = (coords.top + coords.bottom) / 2;
//         int dx, dy;
        
//         dx = coords.left - center_x;
//         dy = coords.top - center_y;
//         coords.left = center_x + dx * cos_theta - dy * sin_theta;
//         coords.top = center_y + dx * sin_theta + dy * cos_theta;

//         dx = coords.right - center_x;
//         dy = coords.bottom - center_y;
//         coords.right = center_x + dx * cos_theta - dy * sin_theta;
//         coords.bottom = center_y + dx * sin_theta + dy * cos_theta;
//       }
//     }
//   } else {
//     printf("[DEBUG] Not enough lines found for grid\n");
//   }

//   free(lines);
//   free(h_rhos);
//   free(v_rhos);

//   return coords;
// }