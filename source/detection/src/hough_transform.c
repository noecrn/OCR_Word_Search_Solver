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

void hough_transform_binary(SDL_Surface* surface, HoughLine* lines, int* num_lines) {
    SDL_LockSurface(surface);
    Uint32* pixels = (Uint32*)surface->pixels;
    
    const int width = surface->w;
    const int height = surface->h;
    const int diagonal = sqrt(width*width + height*height);
    const int num_thetas = 180;
    
    int* accumulator = calloc(2*diagonal * num_thetas, sizeof(int));
    if (!accumulator) {
        SDL_UnlockSurface(surface);
        return;
    }
    
    // Process black pixels directly (assuming black is 0)
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y*width + x], surface->format, &r, &g, &b);
            
            // Process black pixels (assuming black = 0)
            if(r == 0) { 
                for(int t = 0; t < num_thetas; t++) {
                    float theta = t * M_PI / num_thetas;
                    int rho = x*cos(theta) + y*sin(theta);
                    rho += diagonal;
                    accumulator[rho*num_thetas + t]++;
                }
            }
        }
    }
    
    // Find peaks
    const int threshold = 50; // May need adjustment based on image
    *num_lines = 0;
    for(int r = 0; r < 2*diagonal; r++) {
        for(int t = 0; t < num_thetas; t++) {
            if(accumulator[r*num_thetas + t] > threshold) {
                lines[*num_lines].rho = r - diagonal;
                lines[*num_lines].theta = t * M_PI / num_thetas;
                (*num_lines)++;
            }
        }
    }
    
    SDL_UnlockSurface(surface);
    free(accumulator);
}

GridCoords detect_grid_coords(SDL_Surface* image) {
    GridCoords coords = {0, 0, 0, 0};
    HoughLine* lines = malloc(1000 * sizeof(HoughLine));
    if (!lines) {
        return coords;
    }
    int num_lines = 0;
    
    hough_transform_binary(image, lines, &num_lines);
    
    // Arrays for storing rho values
    int* h_rhos = malloc(num_lines * sizeof(int));
    int* v_rhos = malloc(num_lines * sizeof(int));
    if (!h_rhos || !v_rhos) {
        free(lines);
        free(h_rhos);
        free(v_rhos);
        return coords;
    }
    int h_count = 0, v_count = 0;
    
    // Separate horizontal and vertical lines
    for(int i = 0; i < num_lines; i++) {
        float theta = lines[i].theta;
        if(fabs(theta) < 0.1 || fabs(theta - M_PI) < 0.1) {
            h_rhos[h_count++] = lines[i].rho;
        }
        else if(fabs(theta - M_PI/2) < 0.1) {
            v_rhos[v_count++] = lines[i].rho;
        }
    }
    
    // Sort rhos
    for(int i = 0; i < h_count-1; i++) {
        for(int j = 0; j < h_count-i-1; j++) {
            if(h_rhos[j] > h_rhos[j+1]) {
                int temp = h_rhos[j];
                h_rhos[j] = h_rhos[j+1];
                h_rhos[j+1] = temp;
            }
        }
    }
    
    for(int i = 0; i < v_count-1; i++) {
        for(int j = 0; j < v_count-i-1; j++) {
            if(v_rhos[j] > v_rhos[j+1]) {
                int temp = v_rhos[j];
                v_rhos[j] = v_rhos[j+1];
                v_rhos[j+1] = temp;
            }
        }
    }
    
    // Get grid boundaries
    if(h_count >= 2 && v_count >= 2) {
        coords.top = h_rhos[0];
        coords.bottom = h_rhos[h_count-1];
        coords.left = v_rhos[0];
        coords.right = v_rhos[v_count-1];
    }
    
    free(lines);
    free(h_rhos);
    free(v_rhos);
    
    return coords;
}