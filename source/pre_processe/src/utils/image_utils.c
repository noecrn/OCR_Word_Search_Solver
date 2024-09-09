#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <err.h>

void rotate(double angle)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        errx(EXIT_FAILURE, "Failed to initialize SDL: %s", SDL_GetError());
    }

    SDL_Surface *imageSurface = IMG_Load("temp.png");
    if (!imageSurface)
    {
        errx(EXIT_FAILURE, "Failed to load image: %s", IMG_GetError());
    }

    // int imageWidth = imageSurface->w;
    // int imageHeight = imageSurface->h;

 
    double angle_rad = angle * M_PI / 180;
    int new_width = ceil(fabs(imageSurface->w * cos(angle_rad)) + fabs(imageSurface->h * sin(angle_rad)));
    int new_height = ceil(fabs(imageSurface->w * sin(angle_rad)) + fabs(imageSurface->h * cos(angle_rad)));
    SDL_Surface *rotated = SDL_CreateRGBSurface(0, new_width, new_height, 32, 0, 0, 0, 0);

    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(rotated);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect dstRect = {(new_width-(imageSurface->w))/2, (new_height-(imageSurface->h))/2, imageSurface->w, imageSurface->h};
    SDL_RenderCopyEx(renderer, SDL_CreateTextureFromSurface(renderer, imageSurface), NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
    SDL_RenderPresent(renderer);

    SDL_DestroyRenderer(renderer);
    IMG_SavePNG(rotated, "temp.png"); 
    SDL_FreeSurface(rotated);
}
