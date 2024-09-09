#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>

void rotate(const char* inputPath, const char* outputPath, double angle);
void resizeImage(const char* inputPath, const char* outputPath, int minSideSize);

// Function to rotate an image
void rotate(const char* inputPath, const char* outputPath, double angle) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        errx(EXIT_FAILURE, "Failed to initialize SDL: %s", SDL_GetError());
    }

    // Load the image from the given input path
    SDL_Surface *imageSurface = IMG_Load(inputPath);
    if (!imageSurface) {
        errx(EXIT_FAILURE, "Failed to load image: %s", IMG_GetError());
    }

    // Calculate the angle in radians and the new dimensions
    double angle_rad = angle * M_PI / 180;
    int new_width = ceil(fabs(imageSurface->w * cos(angle_rad)) + fabs(imageSurface->h * sin(angle_rad)));
    int new_height = ceil(fabs(imageSurface->w * sin(angle_rad)) + fabs(imageSurface->h * cos(angle_rad)));

    // Create a new SDL_Surface for the rotated image
    SDL_Surface *rotatedSurface = SDL_CreateRGBSurface(0, new_width, new_height, 32, 0, 0, 0, 0);
    if (!rotatedSurface) {
        SDL_FreeSurface(imageSurface);
        errx(EXIT_FAILURE, "Failed to create rotated surface: %s", SDL_GetError());
    }

    // Create a software renderer
    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(rotatedSurface);
    if (!renderer) {
        SDL_FreeSurface(imageSurface);
        SDL_FreeSurface(rotatedSurface);
        errx(EXIT_FAILURE, "Failed to create renderer: %s", SDL_GetError());
    }

    // Clear the renderer with a background color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Create a texture from the original surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    if (!texture) {
        SDL_DestroyRenderer(renderer);
        SDL_FreeSurface(imageSurface);
        SDL_FreeSurface(rotatedSurface);
        errx(EXIT_FAILURE, "Failed to create texture: %s", SDL_GetError());
    }

    // Render the texture to the rotated surface
    SDL_Rect dstRect = {(new_width - imageSurface->w) / 2, (new_height - imageSurface->h) / 2, imageSurface->w, imageSurface->h};
    SDL_RenderCopyEx(renderer, texture, NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);

    // Save the rotated image to the output path
    if (IMG_SavePNG(rotatedSurface, outputPath) != 0) {
        errx(EXIT_FAILURE, "Failed to save rotated image: %s", IMG_GetError());
    }

    // Clean up resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(imageSurface);
    SDL_FreeSurface(rotatedSurface);
    
    // Quit SDL
    SDL_Quit();
}

// Function to resize an image while preserving aspect ratio
void resizeImage(const char* inputPath, const char* outputPath, int minSideSize) {
    // Initialize SDL and SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }

    // Load the input image into an SDL_Surface
    SDL_Surface* originalImage = IMG_Load(inputPath);
    if (!originalImage) {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Get original image dimensions
    int originalWidth = originalImage->w;
    int originalHeight = originalImage->h;

    // Calculate new dimensions while maintaining aspect ratio
    int newWidth, newHeight;
    if (originalWidth < originalHeight) {
        newWidth = minSideSize;
        newHeight = (minSideSize * originalHeight) / originalWidth;
    } else {
        newHeight = minSideSize;
        newWidth = (minSideSize * originalWidth) / originalHeight;
    }

    // Create a new SDL_Surface with the resized dimensions
    SDL_Surface* resizedImage = SDL_CreateRGBSurface(
        0, newWidth, newHeight, originalImage->format->BitsPerPixel,
        originalImage->format->Rmask, originalImage->format->Gmask,
        originalImage->format->Bmask, originalImage->format->Amask
    );

    if (!resizedImage) {
        printf("SDL_CreateRGBSurface Error: %s\n", SDL_GetError());
        SDL_FreeSurface(originalImage);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Scale the original image to the new surface
    SDL_Rect srcRect = {0, 0, originalWidth, originalHeight};
    SDL_Rect dstRect = {0, 0, newWidth, newHeight};
    if (SDL_BlitScaled(originalImage, &srcRect, resizedImage, &dstRect) != 0) {
        printf("SDL_BlitScaled Error: %s\n", SDL_GetError());
        SDL_FreeSurface(originalImage);
        SDL_FreeSurface(resizedImage);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // Save the resized image to the output path
    if (IMG_SavePNG(resizedImage, outputPath) != 0) {
        printf("IMG_SavePNG Error: %s\n", IMG_GetError());
    } else {
        printf("Image saved successfully to: %s\n", outputPath);
    }

    // Free the surfaces
    SDL_FreeSurface(originalImage);
    SDL_FreeSurface(resizedImage);

    // Quit SDL_image and SDL
    IMG_Quit();
    SDL_Quit();
}