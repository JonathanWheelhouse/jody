#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "surface.h"

SDL_Texture *load_image(SDL_Renderer *renderer, const char *file_name)
{
    SDL_Surface *surface = IMG_Load(file_name);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't create surface from image '%s': %s",
                     file_name, SDL_GetError());
        return NULL; /* Return NULL instead of exit() — caller handles cleanup */
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface); /* SDL3: SDL_DestroySurface, not SDL_FreeSurface */

    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't create texture from surface: %s",
                     SDL_GetError());
        return NULL;
    }

    return texture;
}
