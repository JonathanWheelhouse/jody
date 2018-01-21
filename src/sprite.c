#include <SDL_image.h>
#include <string.h>
#include <stdbool.h>
#include "surface.h"
#include "sprite.h"
#include "gamedefs.h"
#include "util.h"

struct SDL_Surface *image_frame(char *line, const char *dir)
{
	char name[255];
	int r=0, g=0, b=0;
	int match = sscanf(line, "%s %d %d %d", name, &r, &g, &b);
	if (match != 4)
		return NULL;

	char filename[255];
	sprintf(filename, "%s/%s", dir, name);

	return load_image(filename);
}

struct sprite_base *base_init(const char *dir)
{
	char *filename;
	if (asprintf(&filename, "%s/info", dir) < 0)
		return NULL;

	FILE *fp;
	if ((fp=fopen(filename, "r")) == NULL) {
		fprintf(stderr, "ERROR opening file %s\n\n", filename);
		return NULL;
	}

	struct sprite_base *base = xmalloc(sizeof(struct sprite_base));
	base->is_built = base->frames_count = base->image_width = base->image_height = 0;
	base->can_be_branded = false;

	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	if (getline(&line, &len, fp) == -1) {
		fprintf(stderr, "ERROR reading first line of file %s\n\n", filename);
		return NULL;
	}
	sscanf(line, "FILES: %d", &base->frames_count);
	base->frames = xcalloc(base->frames_count, sizeof(struct sprite_frame *));

	if (getline(&line, &len, fp) == -1) {
		fprintf(stderr, "Error reading second line of file %s\n\n", filename);
		return NULL;
	}

	char can_be_branded[2];
	if (sscanf(line, "can_be_branded: %s", can_be_branded) != -1)
		if (can_be_branded[0] == 'y' || can_be_branded[0] == 'Y')
			base->can_be_branded = true;

	base->is_built = 1;
	int count = 0;

	len = 0;
	while ((nread = getline(&line, &len, fp)) != -1 && count < base->frames_count) {
		if (line[0] != '#' && line[0] != '\r' && line[0] != '\0' && line[0] != '\n' && strlen(line) != 0) {

			struct sprite_frame *sprite_frame = xcalloc(1, sizeof(struct sprite_frame));
			base->frames[count] = sprite_frame;

			base->frames[count]->image = image_frame(line, dir);
			if (!base->frames[count]->image)
				return NULL;

			if (!base->image_width)
				base->image_width = base->frames[count]->image->w;
			if (!base->image_height)
				base->image_height = base->frames[count]->image->h;

			count++;
		}
	}

	free(line);
	fclose(fp);

	printf("file [%s] base->image_width [%d] base->image_height [%d]\n", filename, base->image_width, base->image_height);

	return base;
}

struct sprite *sprite_init(struct sprite_base *base, SDL_Surface *screen)
{
	struct sprite *sprite = xmalloc(sizeof(struct sprite));
	sprite->frame_index = 0;
	sprite->x = 0;
	sprite->y = 0;
	sprite->prev_x = 0;
	sprite->prev_y = 0;
	sprite->is_animating = false;
	sprite->is_drawn = false;
	sprite->is_branded = false;
	sprite->speed = 0;
	sprite->last_update = 0;

	sprite->sprite_base = base;

	if (sprite->sprite_base->is_built) {
		if (sprite->sprite_base->frames_count > 1)
			sprite->is_animating = true;
	}
	sprite->screen = screen;
	return sprite;
}

void free_sprite(struct sprite *sprite)
{
	if (sprite) {
		SDL_Surface *brand = sprite->brand;
		if (brand)
			SDL_FreeSurface(brand);
		struct sprite_base *sprite_base = sprite->sprite_base;
		if (sprite_base) {
			struct sprite_frame **frames = sprite_base->frames;
			if (frames) {
				for (int i = 0; i < sprite_base->frames_count; i++) {
					if (sprite_base->frames[i]) {
						struct sprite_frame *sprite_frame = sprite_base->frames[i];
						SDL_Surface *image = sprite_frame->image;
						if (image)
							SDL_FreeSurface(image) ;
						free(sprite_frame);
					}
				}
				free(frames);
			}
			free(sprite_base);
		}
		free(sprite);
	}
}

void draw(struct sprite *sprite, SDL_Surface *brand)
{
	if (sprite->is_animating == true) {
		sprite->frame_index++;
		/* Make the animal face the direction it is going. */
		int half = (sprite->sprite_base->frames_count - 1) / 2;
		if (sprite->frame_index > half)
			sprite->frame_index = 0;
		sprite->last_update = SDL_GetTicks();
	}

	if (sprite->is_drawn == false)
		sprite->is_drawn = true;

	SDL_Rect dest;
	dest.x = (int)sprite->x; dest.y = (int)sprite->y;
	SDL_BlitSurface(sprite->sprite_base->frames[sprite->frame_index]->image, NULL, sprite->screen, &dest);
	if (sprite->is_branded) {
		SDL_Surface *brand_to_put = NULL;
		if (sprite->brand != NULL)
			brand_to_put = sprite->brand;
		else if (brand != NULL) {
			sprite->brand = brand;
			brand_to_put = brand;
		}
		if (brand_to_put != NULL) {
			dest.x += 20; dest.y += 20;
			SDL_BlitSurface(brand_to_put, NULL, sprite->screen, &dest);
		}
	}
}

void set_speed(struct sprite *sprite, float speed) { sprite->speed = speed; }
float get_speed(struct sprite *sprite) { return sprite->speed; }

void xadd(struct sprite *sprite, double displacement)
{
	sprite->x += displacement;
	if (sprite->x > SCREEN_WIDTH)
		sprite->x = 0;
}

void set(struct sprite *sprite, double x, double y) { sprite->x = x; sprite->y = y; }
