#include <SDL_image.h>
#include "surface.h"

struct SDL_Surface *load_image(const char *file_name)
{
	SDL_Surface *surface;
	if ((surface = IMG_Load(file_name)) == NULL)
		return NULL;

	SDL_Surface *optimized_surface;
	if ((optimized_surface = SDL_DisplayFormatAlpha(surface)) == NULL) {
		SDL_FreeSurface(surface);
		return NULL;
	}
	SDL_FreeSurface(surface);

	return optimized_surface;
}
