#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL/SDL.h>

struct sprite_frame
{
  SDL_Surface *image;
};

struct sprite_base
{
	struct sprite_frame **frames;
	int is_built, frames_count, image_width, image_height;
	bool can_be_branded;
};

struct sprite
{
	int frame_index;
	double x, y, prev_x, prev_y;
	bool is_animating;
	bool is_drawn;
	bool is_branded;
	SDL_Surface *brand;
	float speed;
	long last_update;
	struct sprite_base *sprite_base;
	SDL_Surface *screen;
};


struct sprite_base *base_init(const char *dir);
struct sprite *sprite_init(struct sprite_base *base, SDL_Surface *screen);
void free_sprite(struct sprite *sprite);

void draw(struct sprite *sprite, SDL_Surface *brand);

void set_frame_index(struct sprite *sprite, int frame_index);
int get_frame_index();

void set_speed(struct sprite *sprite, float speed);
float get_speed(struct sprite *sprite);

void toggle_is_animating(struct sprite *sprite);
void start_animating(struct sprite *sprite);
void stop_animating(struct sprite *sprite);
void rewind_frame(struct sprite *sprite);

void xadd(struct sprite *sprite, double displacement);
void yadd(struct sprite *sprite, double nr);
void xset(struct sprite *sprite, double nr);
void yset(struct sprite *sprite, double nr);
void set(struct sprite *sprite, double x, double y);

#endif
