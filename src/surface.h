#ifndef __SURFACE_H__
#define __SURFACE_H__

#include <SDL3/SDL.h>

SDL_Texture *load_image(SDL_Renderer *renderer, const char *file_name);

#endif
