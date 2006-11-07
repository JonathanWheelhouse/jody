#include <SDL_image.h>
#include <string.h>
#include "sprite.h"
#include "gamedefs.h"

struct sprite_base *base_init(const char *dir)
{
	char buffer[255];
	char filename[255];
	char name[255];
	int pause=0, r=0, g=0, b=0;
	FILE *fp;

	sprintf(filename, "%s/info", dir);

	if ((fp=fopen(filename, "r")) == NULL) {
		fprintf(stderr, "ERROR opening file %s\n\n", filename);
		return NULL;
	}


	struct sprite_base *base = malloc(sizeof(struct sprite_base));
	base->is_built = base->frames_count = base->image_width = base->image_height = 0;

	fgets(buffer, 255, fp);
	sscanf(buffer, "FILES: %d", &base->frames_count);

	struct sprite_frame *frames = malloc(base->frames_count * sizeof(struct sprite_frame));
  

	base->is_built = 1;
	int count = 0;

	while (!feof(fp) && count < base->frames_count) {
		fgets(buffer, 255, fp);
		if (buffer[0] != '#' && buffer[0] != '\r' && buffer[0] != '\0' && buffer[0] != '\n' && strlen(buffer) != 0) {

			int match = sscanf(buffer, "%s %d %d %d %d", name, &pause, &r, &g, &b);
			if (match != 5)
				return NULL;

			sprintf(filename, "%s/%s", dir, name);

			SDL_Surface *temp;
			if((temp = IMG_Load(filename)) == NULL)
				return NULL;
			if(r >= 0)
				SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, r, g, b));

/* 			frames[count].image = SDL_DisplayFormat(temp); */
			frames[count].image = temp;
/* 			SDL_FreeSurface(temp); */

			frames[count].pause = pause;
			if (!base->image_width)
				base->image_width = frames[count].image->w;
			if (!base->image_height)
				base->image_height = frames[count].image->w;

			count++;
		}
	}
	fclose(fp);

	base->frames = frames;
	return base;
}

struct sprite *sprite_init(struct sprite_base *base, SDL_Surface *screen)
{
	struct sprite *sprite = malloc(sizeof(struct sprite));
	sprite->frame_index = 0;
	sprite->x = 0;
	sprite->y = 0;
	sprite->prev_x = 0;
	sprite->prev_y = 0;
	sprite->is_animating = 0;
	sprite->is_drawn = 0;
	sprite->speed = 0;
	sprite->last_update = 0;

	sprite->sprite_base = base;

	if (sprite->sprite_base->is_built) {
		if (sprite->sprite_base->frames_count > 1)
			sprite->is_animating = 1;
	}
	sprite->screen = screen;
	return sprite;
}

void free_sprite(struct sprite *sprite)
{
	if (sprite) {
		struct sprite_base *sprite_base = sprite->sprite_base;
		if (sprite_base) {
			struct sprite_frame *frames = sprite_base->frames;
			if (frames)
				for (int i = 0; i < sprite_base->frames_count; i++) {
					SDL_Surface *image = (frames + i)->image;
/*FIXME - dunno why but SDL_FreeSurface gets a segmentation fault although gdb shows image is valid */
/* 					if (image) */
/* 						SDL_FreeSurface(image); */
				}
				free(frames);
			free(sprite_base);
		}
		free(sprite);
	}
}

void draw(struct sprite *sprite)
{
	if (sprite->is_animating == 1) {
		if (sprite->last_update + sprite->sprite_base->frames[sprite->frame_index].pause * sprite->speed < SDL_GetTicks()) {
			sprite->frame_index++;
			if (sprite->frame_index > sprite->sprite_base->frames_count - 1)
				sprite->frame_index = 0;
			sprite->last_update = SDL_GetTicks();
		}
	}

	if(sprite->is_drawn == 0)
		sprite->is_drawn = 1;

	SDL_Rect dest;
	dest.x = sprite->x; dest.y = sprite->y;
	SDL_BlitSurface(sprite->sprite_base->frames[sprite->frame_index].image, NULL, sprite->screen, &dest);
}

void set_frame_index(struct sprite *sprite, int frame_index) { sprite->frame_index = frame_index; }
int get_frame_index(struct sprite *sprite) { return sprite->frame_index; }

void set_speed(struct sprite *sprite, float speed) { sprite->speed = speed; }
float get_speed(struct sprite *sprite) { return sprite->speed; }

void toggle_is_animating(struct sprite *sprite) { sprite->is_animating = !sprite->is_animating; }
void start_animating(struct sprite *sprite) { sprite->is_animating = 1; }
void stop_animating(struct sprite *sprite) { sprite->is_animating = 0; }
void rewind_frame(struct sprite *sprite) { sprite->frame_index = 0; }

void xadd(struct sprite *sprite, int displacement)
{
	sprite->x += displacement;
	if(sprite->x > SCREEN_WIDTH)
		sprite->x = 0;
}

void yadd(struct sprite *sprite, int nr) { sprite->y += nr; }
void xset(struct sprite *sprite, int nr) { sprite->x = nr; }
void yset(struct sprite *sprite, int nr) { sprite->y = nr; }
void set(struct sprite *sprite, int x, int y) { sprite->x = x; sprite->y = y; }
