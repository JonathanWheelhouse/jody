#define _GNU_SOURCE

#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include "img_dir.h"
#include "gamedefs.h"
#include "resources.h"
#include "util.h"

/* global variables */
int fullscreen = 0;

double time_scale = 0;

/* Prototypes */
static void getargs(int argc, char *argv[]);
static void usage(int ret);
static void play_game(void);
static void handle_events(int *quit, int *pause);
static void draw(SDL_Rect *src, int *pause);
static SDL_Rect calc_dest(struct image *image);

int main(int argc, char *argv[])
{
	 getargs(argc, argv);

	 srand(time(0));

	 setup(fullscreen);

	 /* Load the game's data into globals. */

	 play_game();

	 /* Unload data. */

	 exit(0);
}

static void getargs(int argc, char *argv[])
{
	 int i;

	 for (i = 1; i < argc; i++) {
		  if (strcmp(argv[i], "--fullscreen") == 0 || strcmp(argv[i], "-f") == 0) {
			   fullscreen = 1;
		  } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			   printf(
					"\njody\n"
					"Version " VERSION "\n"
					"Copyright 2005 Jonathan Wheelhouse\n"
					"\n"
					"Game controls:\n"
					"  Keys:\n"
					"    ESC or q       - quit\n"
					"    SPACE or p     - toggle pause\n"
					"    arrow keys     - move person\n"
					"    f              - toggle fullscreen\n"
					"    1              - arrow cursor\n"
					"    2              - wheelhouse cursor\n"
					"    3              - transparent wheelhouse cursor\n"
					"    4              - inverted wheelhouse cursor\n"
					"    5              - black with white lines wheelhouse cursor\n"
					"  Mouse Movement   - Move branding iron.\n"
					"  Any Mouse Button - Brand!\n"
					"\n"
					"Run with \"--usage or -u\" for command-line options...\n"
					"\n");
			   exit(0);
		  } else if (strcmp(argv[i], "--usage") == 0 || strcmp(argv[i], "-u") == 0)
			   usage(0);
		  else
			   usage(1);
	 }
}

static void play_game(void)
{

	 /* keep track of frames and time */
	 int prev_ticks = 0, cur_ticks = 0;
	 int start_time, end_time;
	 int frames_drawn = 0;
	 prev_ticks = SDL_GetTicks();
	 start_time = time(NULL);


	 /* 126 = width of 1 cow of the 3 cow tile; 95 = height of cow */
	 SDL_Rect src;
	 src.w = 126;
	 src.h = 95;
	 src.x = 0;
	 src.y = 0;

	 int quit = 0;
	 int pause = 0;
	 while (quit == 0) {

		  /* Determine how many milliseconds have passed since
			 the last frame, and update our motion scaling. */

		  prev_ticks = cur_ticks;
		  cur_ticks = SDL_GetTicks();
		  time_scale = (double)(cur_ticks-prev_ticks)/30.0;

		  handle_events(&quit, &pause);


		  /* Redraw everything. */
/* 		  DrawBackground(screen); */
		  draw(&src, &pause);

		  /* Flip the page. */
		  SDL_Flip(screen);

		  frames_drawn++;

	 }

	 end_time = time(NULL);
	 if (start_time == end_time) end_time++;

	 /* Display the average framerate. */
	 printf("Drew %i frames in %i seconds, for a framerate of %.2f fps.\n",
			frames_drawn,
			end_time-start_time,
			(float)frames_drawn/(float)(end_time-start_time));

}

static void usage(int ret)
{
	 FILE *fs;
  
	 fs = (ret == 1) ? stderr : stdout;
	 fprintf(fs,
			 "\nUsage: jody [--fullscreen] | [--help (-h) | --usage (-u)]\n\n"
			 " --fullscreen or -f - Display in full screen instead of a window, if possible.\n"
			 "\n");
  	 exit(ret);
}

static void handle_events(int *quit, int *pause)
{
	 SDL_Event event;
	 while (SDL_PollEvent(&event) > 0) {
		  if (event.type == SDL_QUIT) {
			   *quit = 1;
		  } else if (event.type == SDL_KEYDOWN) {
			   switch (event.key.keysym.sym) {
			   case SDLK_ESCAPE:
			   case SDLK_q:
					*quit = 1;
					break;
			   case SDLK_1:
					SDL_SetCursor(cursor_arrow);
					break;
			   case SDLK_2:
					SDL_SetCursor(cursor_wheelhouse);
					break;
			   case SDLK_3:
					SDL_SetCursor(cursor_wheelhouse_transparent);
					break;
			   case SDLK_4:
					SDL_SetCursor(cursor_wheelhouse_inverted);
					break;
			   case SDLK_5:
					SDL_SetCursor(cursor_wheelhouse_black_with_white_lines);
					break;
			   case SDLK_f:
					SDL_WM_ToggleFullScreen(screen);
					break;
			   case SDLK_p:
			   case SDLK_SPACE:
					*pause = !*pause;
					break;
			   default:
					break;
			   }
		  }
	 }
}

static void draw(SDL_Rect *src, int *pause)
{
	 SDL_Rect dest;

	 /* background */
	 dest.x = 0;
	 dest.y = 0;
	 dest.w = (images[IMG_BACKGROUND].surface)->w;
	 dest.h = (images[IMG_BACKGROUND].surface)->h;
	 SDL_BlitSurface(images[IMG_BACKGROUND].surface, NULL, screen, &dest);

	 int i;
	 for (i = 1; i < NUM_IMAGES; i++) {

/* 		  /\* cow black tile *\/ */
/* 		  /\* struct for storing game state like pig.c *\/ */
/* 		  /\* struct for storing tile data per animal type */
/* 			 method per animal type for moving */
/* 			 struct per animal to store x & y coords */
/* 			 calculate moves outside draw: */
/* 			 - foreach animal type */
/* 			 - foreach animal calculate move */
/* 		  *\/ */
/* 		  if (!*pause) { */
/* 			   /\* Move to the next cow in the 3 cow tile *\/ */
/* 			   int cow3_x = 252; */
/* 			   int cow_width = 126; */
/* 			   src->x += cow_width; */
/* 			   if (src->x > cow3_x) */
/* 					src->x = 0; */
		  struct image *image = &images[i];
		  dest.x = image->x;
		  dest.y = image->y;
		  dest.w = image->surface->w;
		  dest.h = image->surface->h;
		  if (!*pause)
			   dest = calc_dest(image);
		  SDL_BlitSurface(image->surface, NULL, screen, &dest);
	 }
}

static SDL_Rect calc_dest(struct image *image)
{
	 SDL_Rect dest;

	 /* Move the cow horizontally. */
	 image->x += 20;
	 if (image->x > SCREEN_WIDTH)
		  image->x = 0;
	 dest.x = image->x;

	 /* Move the cow vertically and randomly. */
	 /* Add check for top and bottom; do something sensible
		like bounce off. */

	 unsigned int odd = random() % 2;
	 if (odd) {
		  image->y += 5;
	 }
	 else {
		  image->y -= 5;
	 }
	 dest.y = image->y;

	 return dest;
}
