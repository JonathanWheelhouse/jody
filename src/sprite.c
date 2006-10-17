#include <SDL_image.h>
#include <string.h>
#include "sprite.h"

struct sprite_base *base_init(char *dir)
{
	char buffer[255];
	char filename[255];
	char name[255];
	int pause=0, r=0, g=0, b=0;
	FILE *fp;

	sprintf(filename, "%s/info", dir);

	if ((fp=fopen(filename, "r")) == NULL) {
		printf("ERROR opening file %s\n\n", filename);
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

struct sprite *init(struct sprite_base *base, SDL_Surface *screen)
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
		sprite->back_replacement = SDL_DisplayFormat(sprite->sprite_base->frames[0].image);
	}
	sprite->screen = screen;
	return sprite;
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

void clear_background(struct sprite *sprite)
{
	if (sprite->is_drawn == 1) { // first time through, this is not done
		SDL_Rect dest;
		dest.x = sprite->prev_x;
		dest.y = sprite->prev_y;
		dest.w = sprite->sprite_base->image_width;
		dest.h = sprite->sprite_base->image_height;
		SDL_BlitSurface(sprite->back_replacement, NULL, sprite->screen, &dest);
	}
}

/* this is misnamed a bit; it really sets back_replacement (which is set to background */
void update_background(struct sprite *sprite)
{
	SDL_Rect srcrect;
	srcrect.w = sprite->sprite_base->image_width;
	srcrect.h = sprite->sprite_base->image_height;
	srcrect.x = sprite->x;
	srcrect.y = sprite->y;
	sprite->prev_x = sprite->x;
	sprite->prev_y = sprite->y;
	/* first time, back_replacement is set to the background (as background has just been set to screen */
	SDL_BlitSurface(sprite->screen, &srcrect, sprite->back_replacement, NULL);
}

void set_frame_index(struct sprite *sprite, int frame_index) { sprite->frame_index = frame_index; }
int get_frame_index(struct sprite *sprite) { return sprite->frame_index; }

void set_speed(struct sprite *sprite, float speed) { sprite->speed = speed; }
float get_speed(struct sprite *sprite) { return sprite->speed; }

void toggle_is_animating(struct sprite *sprite) { sprite->is_animating = !sprite->is_animating; }
void start_animating(struct sprite *sprite) { sprite->is_animating = 1; }
void stop_animating(struct sprite *sprite) { sprite->is_animating = 0; }
void rewind_frame(struct sprite *sprite) { sprite->frame_index = 0; }

void xadd(struct sprite *sprite, int nr) { sprite->x += nr; }
void yadd(struct sprite *sprite, int nr) { sprite->y += nr; }
void xset(struct sprite *sprite, int nr) { sprite->x = nr; }
void yset(struct sprite *sprite, int nr) { sprite->y = nr; }
void set(struct sprite *sprite, int x, int y) { sprite->x = x; sprite->y = y; }
