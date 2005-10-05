#define _GNU_SOURCE
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

SDL_Cursor *cursor_arrow;
SDL_Cursor *cursor_wheelhouse;
SDL_Cursor *cursor_wheelhouse_transparent;
SDL_Cursor *cursor_wheelhouse_inverted;
SDL_Cursor *cursor_wheelhouse_black_with_white_lines;

static void seticon(void);

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

static void setup_img(void)
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

void setup(int fullscreen)
{
	 if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		  printf("Unable to initialize SDL: %s\n", SDL_GetError());
		  exit(1);
	 }

	 atexit(SDL_Quit);

	 seticon();

	 if (SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF |
						  (fullscreen ? SDL_FULLSCREEN : 0)) == NULL) {
		  printf("Unable to set video mode: %s\n", SDL_GetError());
		  exit(1);
	 }

	 screen = SDL_GetVideoSurface();

	 SDL_WM_SetCaption("jody", "jody");

	 /* Setup the cursors. */
	 cursor_arrow = create_cursor_arrow();
	 cursor_wheelhouse = create_cursor_wheelhouse();
	 cursor_wheelhouse_transparent = create_cursor_wheelhouse_transparent();
	 cursor_wheelhouse_inverted = create_cursor_wheelhouse_inverted();
	 cursor_wheelhouse_black_with_white_lines = create_cursor_wheelhouse_black_with_white_lines();

	 setup_img();
}

static void seticon(void)
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

SDL_Cursor *create_cursor_arrow(void)
{
	 /* Stolen from the mailing list */
	 /* Creates a new mouse cursor from an XPM */


	 /* XPM */
	 static const char *arrow[] = {
		  /* width height num_colors chars_per_pixel */
		  "    32    32        3            1",
		  /* colors */
		  "X c #000000",
		  ". c #ffffff",
		  "  c None",
		  /* pixels */
		  "X                               ",
		  "XX                              ",
		  "X.X                             ",
		  "X..X                            ",
		  "X...X                           ",
		  "X....X                          ",
		  "X.....X                         ",
		  "X......X                        ",
		  "X.......X                       ",
		  "X........X                      ",
		  "X.....XXXXX                     ",
		  "X..X..X                         ",
		  "X.X X..X                        ",
		  "XX  X..X                        ",
		  "X    X..X                       ",
		  "     X..X                       ",
		  "      X..X                      ",
		  "      X..X                      ",
		  "       XX                       ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "0,0"
	 };

	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (arrow[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
					mask[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(arrow[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

static const char *wheelhouse[] = {
	 /* width height num_colors chars_per_pixel */
	 "    32    32        3            1",
	 /* colors */
	 "X c #000000",
	 ". c #ffffff",
	 "  c None",
	 /* pixels */
	 "       X                X       ",
	 "     X.X.X            X...X     ",
	 "   X...X...X         X.....X    ",
	 " X.....X.....X      X.......X   ",
	 "X......X......X    X.........X  ",
	 "X......X......X   X...........X ",
	 "XXXXXXXXXXXXXXX  XXXXXXXXXXXXXXX",
	 "X......X......X  X.............X",
	 "X......X......X  X.............X",
	 " X.....X.....X   X.............X",
	 "   X...X...X     X.............X",
	 "     X.X.X       X.............X",
	 "       X         XXXXXXXXXXXXXXX",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "0,0"
};

SDL_Cursor *create_cursor_wheelhouse(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
					mask[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
SDL_Cursor *create_cursor_wheelhouse_transparent(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
/*
 * Make it transparent rather than white.
 */
/* 				mask[i] |= 0x01; */
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
SDL_Cursor *create_cursor_wheelhouse_inverted(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
					data[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
SDL_Cursor *create_cursor_wheelhouse_black_with_white_lines(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					mask[i] |= 0x01;
					break;
			   case '.':
					data[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
