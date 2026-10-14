#define _GNU_SOURCE

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sprite.h"
#include "surface.h"
#include "gamedefs.h"
#include "util.h"

static SDL_Texture *image_frame(SDL_Renderer *renderer, const char *line,
                                 const char *dir)
{
    char name[255];
    int r = 0, g = 0, b = 0;
    int match = sscanf(line, "%254s %d %d %d", name, &r, &g, &b);
    if (match < 1)
        return NULL;

    char *filename = NULL;
    if (asprintf(&filename, "%s/%s", dir, name) < 0)
        return NULL;

    SDL_Texture *tex = load_image(renderer, filename);
    free(filename);
    return tex;
}

struct sprite_base *base_init(SDL_Renderer *renderer, const char *dir)
{
    char *filename = NULL;
    if (asprintf(&filename, "%s/info", dir) < 0)
        return NULL;

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "ERROR opening file %s\n", filename);
        free(filename);
        return NULL;
    }

    struct sprite_base *base = xmalloc(sizeof(struct sprite_base));
    base->is_built = 0;
    base->frames_count = 0;
    base->image_width = 0;
    base->image_height = 0;
    base->can_be_branded = false;
    base->frames = NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    /* Line 1: frame count */
    if ((nread = getline(&line, &len, fp)) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "ERROR reading first line of %s\n", filename);
        goto error;
    }
    sscanf(line, "FILES: %d", &base->frames_count);
    if (base->frames_count <= 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Invalid frames_count in %s\n", filename);
        goto error;
    }
    base->frames = xcalloc((size_t)base->frames_count,
                           sizeof(struct sprite_frame *));

    /* Line 2: can_be_branded flag */
    if ((nread = getline(&line, &len, fp)) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "ERROR reading second line of %s\n", filename);
        goto error;
    }
    {
        char can_be_branded[2] = {0};
        if (sscanf(line, "can_be_branded: %1s", can_be_branded) == 1)
            if (can_be_branded[0] == 'y' || can_be_branded[0] == 'Y')
                base->can_be_branded = true;
    }

    /* Remaining lines: frame images */
    int count = 0;
    len = 0;
    while ((nread = getline(&line, &len, fp)) != -1 &&
           count < base->frames_count) {
        /* Skip comments and blank lines */
        if (line[0] == '#' || line[0] == '\r' || line[0] == '\0' ||
            line[0] == '\n' || strlen(line) == 0)
            continue;

        struct sprite_frame *sprite_frame =
            xcalloc(1, sizeof(struct sprite_frame));
        sprite_frame->image = image_frame(renderer, line, dir);
        if (!sprite_frame->image) {
            free(sprite_frame);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Failed to load frame %d for %s\n", count, dir);
            goto error;
        }

        /* Query texture dimensions */
        {
            float tw, th;
            SDL_GetTextureSize(sprite_frame->image, &tw, &th);
            sprite_frame->w = (int)tw;
            sprite_frame->h = (int)th;
        }

        base->frames[count] = sprite_frame;

        /* Capture dimensions from first frame */
        if (base->image_width == 0)
            base->image_width = sprite_frame->w;
        if (base->image_height == 0)
            base->image_height = sprite_frame->h;

        count++;
    }

    base->is_built = 1;
    free(line);
    fclose(fp);

    SDL_Log("file [%s] width [%d] height [%d]\n",
            filename ? filename : dir, base->image_width, base->image_height);

    free(filename);
    return base;

error:
    /* Cleanup on any failure path */
    if (base->frames) {
        for (int i = 0; i < base->frames_count; i++) {
            if (base->frames[i]) {
                if (base->frames[i]->image)
                    SDL_DestroyTexture(base->frames[i]->image);
                free(base->frames[i]);
            }
        }
        free(base->frames);
    }
    free(line);
    free(base);
    if (fp) fclose(fp);
    free(filename);
    return NULL;
}

struct sprite *sprite_init(struct sprite_base *base)
{
    struct sprite *sprite = xmalloc(sizeof(struct sprite));
    sprite->frame_index = 0;
    sprite->x = 0.0;
    sprite->y = 0.0;
    sprite->prev_x = 0.0;
    sprite->prev_y = 0.0;
    sprite->is_animating = false;
    sprite->is_drawn = false;
    sprite->is_branded = false;
    sprite->brand = NULL;
    sprite->speed = 0.0f;
    sprite->last_update = 0;
    sprite->sprite_base = base;

    if (base->is_built && base->frames_count > 1)
        sprite->is_animating = true;

    return sprite;
}

void free_sprite_base(struct sprite_base *sprite_base)
{
    if (!sprite_base) return;

    if (sprite_base->frames) {
        for (int i = 0; i < sprite_base->frames_count; i++) {
            if (sprite_base->frames[i]) {
                if (sprite_base->frames[i]->image)
                    SDL_DestroyTexture(sprite_base->frames[i]->image);
                free(sprite_base->frames[i]);
            }
        }
        free(sprite_base->frames);
    }
    free(sprite_base);
}

void free_sprite(struct sprite *sprite)
{
    if (!sprite) return;

    /* The brand texture is borrowed — do NOT destroy it here.
       It's owned by the brands struct. */
    free_sprite_base(sprite->sprite_base);
    free(sprite);
}

void draw(SDL_Renderer *renderer, struct sprite *sprite, SDL_Texture *brand)
{
    if (!sprite || !sprite->sprite_base) return;

    /* Animation */
    if (sprite->is_animating) {
        sprite->frame_index++;
        int half = (sprite->sprite_base->frames_count - 1) / 2;
        if (sprite->frame_index > half)
            sprite->frame_index = 0;
        sprite->last_update = (long)SDL_GetTicks();
    }

    if (!sprite->is_drawn)
        sprite->is_drawn = true;

    /* Draw sprite frame */
    SDL_FRect dest;
    dest.x = (float)sprite->x;
    dest.y = (float)sprite->y;
    dest.w = (float)sprite->sprite_base->image_width;
    dest.h = (float)sprite->sprite_base->image_height;

    SDL_RenderTexture(renderer,
        sprite->sprite_base->frames[sprite->frame_index]->image,
        NULL, &dest);

    /* Draw brand overlay */
    if (sprite->is_branded) {
        SDL_Texture *brand_to_use = sprite->brand ? sprite->brand : brand;
        if (brand_to_use) {
            sprite->brand = brand_to_use;
            float bw, bh;
            SDL_GetTextureSize(brand_to_use, &bw, &bh);
            SDL_FRect brand_dest;
            brand_dest.x = dest.x + 20.0f;
            brand_dest.y = dest.y + 20.0f;
            brand_dest.w = bw;
            brand_dest.h = bh;
            SDL_RenderTexture(renderer, brand_to_use, NULL, &brand_dest);
        }
    }
}

void set_speed(struct sprite *sprite, float speed) { sprite->speed = speed; }
float get_speed(struct sprite *sprite) { return sprite->speed; }

void xadd(struct sprite *sprite, double displacement)
{
    if (!sprite) return;
    sprite->x += displacement;
    if (sprite->x > SCREEN_WIDTH)
        sprite->x = 0.0;
}

void set(struct sprite *sprite, double x, double y)
{
    if (!sprite) return;
    sprite->x = x;
    sprite->y = y;
}
