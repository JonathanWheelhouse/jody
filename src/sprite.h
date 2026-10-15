#pragma once

#include <SDL3/SDL.h>

struct sprite_frame
{
    SDL_Texture *image;
    int w, h;
};

struct sprite_base
{
    struct sprite_frame **frames;
    int is_built;
    int frames_count;
    int image_width;
    int image_height;
    bool can_be_branded;
};

struct sprite
{
    int frame_index;
    double x, y, prev_x, prev_y;
    bool is_animating;
    bool is_drawn;
    bool is_branded;
    SDL_Texture *brand;       /* borrowed pointer — do not destroy */
    float speed;
    long last_update;
    struct sprite_base *sprite_base;
};


struct sprite_base *base_init(SDL_Renderer *renderer, const char *dir);
struct sprite *sprite_init(struct sprite_base *base);
void free_sprite(struct sprite *sprite);
void free_sprite_base(struct sprite_base *sprite_base);

void draw(SDL_Renderer *renderer, struct sprite *sprite, SDL_Texture *brand);

void set_speed(struct sprite *sprite, float speed);
float get_speed(struct sprite *sprite);

void xadd(struct sprite *sprite, double displacement);
void set(struct sprite *sprite, double x, double y);
