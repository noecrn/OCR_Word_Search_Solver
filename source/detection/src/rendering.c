#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

// Function to draw a line on a surface
void draw_line(SDL_Surface *surface, int x, int y, SDL_Color color) {
  // Check if the surface is valid
  if (!surface) {
    printf("Erreur: surface non valide\n");
    return;
  }

  Uint32 pixelColor = SDL_MapRGB(surface->format, color.r, color.g, color.b);

  // If x == -1, draw a horizontal line on the whole width at position y
  if (x == -1) {
    // Check bounds before drawing
    if (y >= 0 && y < surface->h) {
      // Draw the horizontal line
      for (int i = 0; i < surface->w; i++) {
        ((Uint32 *)surface->pixels)[y * surface->w + i] = pixelColor;
      }
    }
  }

  // If y == -1, draw a vertical line on the whole height at position x
  else if (y == -1) {
    // Check bounds before drawing
    if (x >= 0 && x < surface->w) {
      // Draw the vertical line
      for (int i = 0; i < surface->h; i++) {
        ((Uint32 *)surface->pixels)[i * surface->w + x] = pixelColor;
      }
    }
  }

  // If x and y are valid, draw a pixel at position (x, y)
  else {
    // Check bounds before drawing
    if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
      ((Uint32 *)surface->pixels)[y * surface->w + x] = pixelColor;
    }
  }
}

// Function to draw the grid on an SDL surface
void draw_grid(SDL_Surface *surface, int left_bound, int right_bound,
               int top_bound, int bottom_bound, int num_rows, int num_cols) {
  SDL_Color blue = {0, 0, 255, 255};

  float block_width = (float)(right_bound - left_bound) / num_cols;
  float block_height = (float)(bottom_bound - top_bound) / num_rows;

  // Draw vertical lines
  for (int col = 0; col <= num_cols; col++) {
    int x = left_bound + (int)(col * block_width + 0.5f);
    draw_line(surface, x, -1, blue);
  }

  // Draw horizontal lines
  for (int row = 0; row <= num_rows; row++) {
    int y = top_bound + (int)(row * block_height + 0.5f);
    draw_line(surface, -1, y, blue);
  }

  // Draw border square around the grid
  draw_line(surface, -1, top_bound, (SDL_Color){255, 0, 0, 255});
  draw_line(surface, -1, bottom_bound, (SDL_Color){255, 0, 0, 255});
  draw_line(surface, right_bound, -1, (SDL_Color){255, 0, 0, 255});
  draw_line(surface, left_bound, -1, (SDL_Color){255, 0, 0, 255});
}

// Function to draw border square on a surface and check if it's still in the
// bounds
void draw_square(SDL_Surface *surface, int x, int y, int size,
                 SDL_Color color) {
  Uint32 pixelColor = SDL_MapRGB(surface->format, color.r, color.g, color.b);

  // Draw the top and bottom borders
  for (int i = 0; i < size; i++) {
    int top_px = x + i;
    int top_py = y;
    int bottom_px = x + i;
    int bottom_py = y + size - 1;

    // Check bounds before drawing
    if (top_px >= 0 && top_px < surface->w && top_py >= 0 &&
        top_py < surface->h) {
      ((Uint32 *)surface->pixels)[top_py * surface->w + top_px] = pixelColor;
    }

    // Check bounds before drawing
    if (bottom_px >= 0 && bottom_px < surface->w && bottom_py >= 0 &&
        bottom_py < surface->h) {
      ((Uint32 *)surface->pixels)[bottom_py * surface->w + bottom_px] =
          pixelColor;
    }
  }

  // Draw the left and right borders
  for (int i = 0; i < size; i++) {
    int left_px = x;
    int left_py = y + i;
    int right_px = x + size - 1;
    int right_py = y + i;

    // Check bounds before drawing
    if (left_px >= 0 && left_px < surface->w && left_py >= 0 &&
        left_py < surface->h) {
      ((Uint32 *)surface->pixels)[left_py * surface->w + left_px] = pixelColor;
    }

    // Check bounds before drawing
    if (right_px >= 0 && right_px < surface->w && right_py >= 0 &&
        right_py < surface->h) {
      ((Uint32 *)surface->pixels)[right_py * surface->w + right_px] =
          pixelColor;
    }
  }
}