#ifndef BRANDS_H
#define BRANDS_H

#include <SDL/SDL.h>

struct brands
{
    SDL_Surface *brand_wheelhouse;
    SDL_Surface *brand_wheelhouse_transparent;
    SDL_Surface *brand_wheelhouse_inverted;
    SDL_Surface *brand_wheelhouse_black_with_white_lines;
};

struct brands *setup_brands(void);
SDL_Surface *create_wheelhouse_image(void);

#endif
