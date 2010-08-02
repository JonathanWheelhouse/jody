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


extern struct sprite_base *base_init(const char *dir);
extern struct sprite *sprite_init(struct sprite_base *base, SDL_Surface *screen);
extern void free_sprite(struct sprite *sprite);

extern void draw(struct sprite *sprite, SDL_Surface *brand);

extern void set_frame_index(struct sprite *sprite, int frame_index);
extern int get_frame_index();

extern void set_speed(struct sprite *sprite, float speed);
extern float get_speed(struct sprite *sprite);

extern void toggle_is_animating(struct sprite *sprite);
extern void start_animating(struct sprite *sprite);
extern void stop_animating(struct sprite *sprite);
extern void rewind_frame(struct sprite *sprite);

extern void xadd(struct sprite *sprite, double displacement);
extern void yadd(struct sprite *sprite, double nr);
extern void xset(struct sprite *sprite, double nr);
extern void yset(struct sprite *sprite, double nr);
extern void set(struct sprite *sprite, double x, double y);

#endif
