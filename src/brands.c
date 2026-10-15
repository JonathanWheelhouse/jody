#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "surface.h"
#include "util.h"
#include "wheelhouse.h"

#include "brands.h"

static SDL_Texture *create_wheelhouse_image(SDL_Renderer *renderer)
{
    SDL_Surface *surface = IMG_ReadXPMFromArray((char **)wheelhouse);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't read wheelhouse XPM: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface); /* Fixed: leak */
    return texture;
}

struct brands *setup_brands(SDL_Renderer *renderer)
{
    struct brands *brands = xmalloc(sizeof(struct brands));

    brands->brand_wheelhouse = create_wheelhouse_image(renderer);
    if (!brands->brand_wheelhouse) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Unable to create brand_wheelhouse\n");
        free(brands);
        return nullptr;
    }

    brands->brand_wheelhouse_transparent =
        load_image(renderer, jody_image_path("wheelhouse_image_transparent.png"));
    if (!brands->brand_wheelhouse_transparent) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Unable to load brand_wheelhouse_transparent\n");
        SDL_DestroyTexture(brands->brand_wheelhouse);
        free(brands);
        return nullptr;
    }

    brands->brand_wheelhouse_inverted =
        load_image(renderer, jody_image_path("wheelhouse_image_inverted.png"));
    if (!brands->brand_wheelhouse_inverted) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Unable to load brand_wheelhouse_inverted\n");
        SDL_DestroyTexture(brands->brand_wheelhouse);
        SDL_DestroyTexture(brands->brand_wheelhouse_transparent);
        free(brands);
        return nullptr;
    }

    brands->brand_wheelhouse_black_with_white_lines =
        load_image(renderer, jody_image_path("wheelhouse_image_black_with_white_lines.png"));
    if (!brands->brand_wheelhouse_black_with_white_lines) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Unable to load brand_wheelhouse_black_with_white_lines\n");
        SDL_DestroyTexture(brands->brand_wheelhouse);
        SDL_DestroyTexture(brands->brand_wheelhouse_transparent);
        SDL_DestroyTexture(brands->brand_wheelhouse_inverted);
        free(brands);
        return nullptr;
    }

    return brands;
}

void cleanup_brands(struct brands *brands)
{
    if (!brands) return;
    SDL_DestroyTexture(brands->brand_wheelhouse);
    SDL_DestroyTexture(brands->brand_wheelhouse_transparent);
    SDL_DestroyTexture(brands->brand_wheelhouse_inverted);
    SDL_DestroyTexture(brands->brand_wheelhouse_black_with_white_lines);
    free(brands);
}
