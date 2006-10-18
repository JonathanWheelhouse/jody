#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL/SDL.h>

struct sprite_frame
{
  SDL_Surface *image;
  int pause;
};

struct sprite_base
{
  struct sprite_frame *frames;
  int is_built, frames_count, image_width, image_height;
};

struct sprite
{
	int frame_index;
	int x, y, prev_x, prev_y;
	int is_animating;
	int is_drawn;
	float speed;
	long last_update;
	struct sprite_base *sprite_base;
	SDL_Surface *back_replacement;
	SDL_Surface *screen;
};

extern struct sprite_base *base_init(char *dir);
extern struct sprite *init(struct sprite_base *base, SDL_Surface *screen);

extern void draw(struct sprite *sprite);

extern void set_frame_index(struct sprite *sprite, int frame_index);
extern int get_frame_index();

extern void set_speed(struct sprite *sprite, float speed);
extern float get_speed(struct sprite *sprite);

extern void toggle_is_animating(struct sprite *sprite);
extern void start_animating(struct sprite *sprite);
extern void stop_animating(struct sprite *sprite);
extern void rewind_frame(struct sprite *sprite);

extern void xadd(struct sprite *sprite, int nr);
extern void yadd(struct sprite *sprite, int nr);
extern void xset(struct sprite *sprite, int nr);
extern void yset(struct sprite *sprite, int nr);
extern void set(struct sprite *sprite, int x, int y);

#endif
