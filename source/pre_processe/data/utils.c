#include <SDL2/SDL.h>
#include <stddef.h>

static size_t flood_fill_count(SDL_Surface *surface, int x, int y,
                               SDL_Color *visited) {
  // Check boundaries and if pixel was already visited
  if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
    return 0;

  Uint32 *pixels = surface->pixels;
  if (visited[y * surface->w + x].a != 0) // Already visited
    return 0;

  Uint32 pixel = pixels[y * surface->w + x];
  Uint8 r, g, b;
  SDL_GetRGB(pixel, surface->format, &r, &g, &b);

  // If pixel is not black (assuming black is RGB(0,0,0))
  if (r != 0 || g != 0 || b != 0)
    return 0;

  // Mark as visited
  visited[y * surface->w + x].a = 255;

  // Count current pixel and recurse in 4 directions
  return 1 + flood_fill_count(surface, x + 1, y, visited) +
         flood_fill_count(surface, x - 1, y, visited) +
         flood_fill_count(surface, x, y + 1, visited) +
         flood_fill_count(surface, x, y - 1, visited);
}

size_t find_biggest_black_component(SDL_Surface *surface) {
  if (!surface)
    return 0;

  // Create visited array
  SDL_Color *visited = calloc(surface->w * surface->h, sizeof(SDL_Color));
  size_t max_size = 0;

  // Iterate through each pixel
  for (int y = 0; y < surface->h; y++) {
    for (int x = 0; x < surface->w; x++) {
      // If not visited and is black, start flood fill
      if (visited[y * surface->w + x].a == 0) {
        Uint32 pixel = ((Uint32 *)surface->pixels)[y * surface->w + x];
        Uint8 r, g, b;
        SDL_GetRGB(pixel, surface->format, &r, &g, &b);

        if (r == 0 && g == 0 && b == 0) {
          size_t component_size = flood_fill_count(surface, x, y, visited);
          if (component_size > max_size)
            max_size = component_size;
        }
      }
    }
  }

  free(visited);
  return max_size;
}
