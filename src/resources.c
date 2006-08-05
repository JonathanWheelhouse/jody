#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL_image.h>
#include <time.h>
#include "gamedefs.h"
#include "resources.h"
#include "img_dir.h"

SDL_Surface *screen;

struct image images[NUM_IMAGES];

static void load_img(int i);
static void set_start_pos(int i, unsigned int x_scale, unsigned int y_scale);

const char *image_names [NUM_IMAGES] = {
	 IMG_DIR "background.png",
	 IMG_DIR "cloud.png",
	 IMG_DIR "cow_black_tile.png",
	 IMG_DIR "cow_black_left.png",
	 IMG_DIR "cow_black.png",
	 IMG_DIR "cow_brown_left.png",
	 IMG_DIR "cow_brown.png",
	 IMG_DIR "cow_orange_left.png",
	 IMG_DIR "cow_orange.png",
	 IMG_DIR "cow_red_left.png",
	 IMG_DIR "cow_red.png",
	 IMG_DIR "cow_white_left.png",
	 IMG_DIR "cow_white.png",
	 IMG_DIR "cow_yellow_left.png",
	 IMG_DIR "cow_yellow.png",
	 IMG_DIR "cow_yellow_with_red_outline_left.png",
	 IMG_DIR "cow_yellow_with_red_outline.png",
	 IMG_DIR "kangaroo_black_left.png",
	 IMG_DIR "kangaroo_black.png",
	 IMG_DIR "kangaroo_grey_left.png",
	 IMG_DIR "kangaroo_grey.png",
	 IMG_DIR "kangaroo_red_left.png",
	 IMG_DIR "kangaroo_red.png"
};

void seticon(void)
{
	 SDL_Surface *icon = IMG_Load(IMG_DIR "icon.png");
	 if (icon == NULL) {
		  fprintf(stderr,
				  "\nError: I could not load the icon image: %s\n"
				  "The Simple DirectMedia error that occurred was:\n"
				  "%s\n\n", IMG_DIR "icon.png", SDL_GetError());
		  exit(1);
	 }
  
	 int masklen = (((icon -> w) + 7) / 8) * (icon -> h);
	 Uint8 *mask = malloc(masklen * sizeof(Uint8));
	 memset(mask, 0xFF, masklen);
    
	 SDL_WM_SetIcon(icon, mask);
  
	 free(mask);
	 SDL_FreeSurface(icon);
}

void setup_img(void)
{
	 srand (time(0));
	 unsigned int x_scale = RAND_MAX / SCREEN_WIDTH;
	 unsigned int y_scale = RAND_MAX / SCREEN_HEIGHT;

	 int i;
	 for (i = 0; i < NUM_IMAGES; i++) {
		  load_img(i);
		  set_start_pos(i, x_scale, y_scale);
	 }
}

static void load_img(int i)
{
	 SDL_Surface *img = IMG_Load(image_names[i]);
	 if (img == NULL) {
		  fprintf(stderr,
				  "\nError: I couldn't load a graphics file:\n"
				  "%s\n"
				  "The Simple DirectMedia error that occurred was:\n"
				  "%s\n\n", image_names[i], SDL_GetError());
		  exit(1);
	 }
	 images[i].surface = img;
}

static void set_start_pos(int i, unsigned int x_scale, unsigned int y_scale)
{
	  unsigned int x = random();
/* 		  printf("x: %10d", x); */
	  x /= x_scale;
	  images[i].x = x;
	  unsigned int y = random();
/* 		  printf(" y: %10d\n", y); */
	  y /= y_scale;
/* 		  printf("x: %10d y: %10d  scaled\n", x, y); */
	  images[i].y = y;
}

