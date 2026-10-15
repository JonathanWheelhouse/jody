#pragma once

#include <SDL3/SDL.h>

struct brands
{
    SDL_Texture *brand_wheelhouse;
    SDL_Texture *brand_wheelhouse_transparent;
    SDL_Texture *brand_wheelhouse_inverted;
    SDL_Texture *brand_wheelhouse_black_with_white_lines;
};

struct brands *setup_brands(SDL_Renderer *renderer);
void cleanup_brands(struct brands *brands);
