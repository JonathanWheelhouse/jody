#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "gamedefs.h"
#include "resources.h"
#include "cursor.h"
#include "util.h"
#include "sprite.h"
#include "datadir.h"

/* constants */
#define X_DIST 10
#define Y_DIST 1

/* global variables */
int fullscreen = 0;
struct sprite_base *cow_black_base;
struct sprite *cow_black;

double time_scale = 0;

/* Prototypes */
static void getargs(int argc, char *argv[]);
static void usage(int ret);
static void setup(int fullscreen);
static void setup_sprites();
static void play_game(void);
static void handle_events(int *quit, int *pause);
static void main_draw(SDL_Rect *src, int *pause);
static SDL_Rect calc_dest(struct image *image);

static SDL_Surface * image_load(char *file);
static int init_images();
static void draw_image(SDL_Surface *img, int x, int y);
static void draw_background();
static void draw_scene(struct sprite *cow_black);

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
				"Copyright 2006 Jonathan Wheelhouse\n"
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

static void setup(int fullscreen)
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

	setup_sprites();

}

static void setup_sprites()
{
	cow_black_base = base_init(IMG_DIR "cow_black");
	cow_black = init(cow_black_base, screen);
	set(cow_black, 150,300);
	set_speed(cow_black, 1);

 	init_images();
 	draw_background();
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
		time_scale = (double)(cur_ticks-prev_ticks)/50.0;

		handle_events(&quit, &pause);


		/* Redraw everything. */
/* 		  DrawBackground(screen); */
/* 		main_draw(&src, &pause); */

		draw_scene(cow_black);

		/* Flip the page. */
/* 		SDL_Flip(screen); */

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

static void main_draw(SDL_Rect *src, int *pause)
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
		dest.w = image->surface->w;
		dest.h = image->surface->h;
		if (*pause) {
			dest.x = image->x;
			dest.y = image->y;
		} else
			dest = calc_dest(image);

		SDL_BlitSurface(image->surface, NULL, screen, &dest);
	}
}

static SDL_Rect calc_dest(struct image *image)
{
	SDL_Rect dest;

	/* Move the cow horizontally. */
	image->x += X_DIST * time_scale;
	if (image->x > SCREEN_WIDTH)
		image->x = 0;
	dest.x = image->x;

	/* Move the cow vertically and randomly. */
	/* Add check for top and bottom; do something sensible
	   like bounce off. */

	unsigned int odd = random() % 2;
	int dist;
	if (odd)
		dist = Y_DIST;
	else
		dist = - Y_DIST;

	image->y += dist * time_scale;
	if (image->y > SCREEN_HEIGHT || image->y < 0)
		image->y = SCREEN_HEIGHT / 2;
	 
	dest.y = image->y;

	return dest;
}

static SDL_Surface * image_load(char *file)
{
	SDL_Surface *temp1, *temp2;
	temp1 = IMG_Load(file);
	if (temp1 == NULL) {
		fprintf(stderr,
				"\nError: 'static SDL_Surface * image_load' function couldn't load a graphics file:\n"
				"%s\n"
				"The Simple DirectMedia error that occurred was:\n"
				"%s\n\n", file, SDL_GetError());
		exit(1);
	}
	temp2 = SDL_DisplayFormat(temp1);
	SDL_FreeSurface(temp1);
	return temp2;
}

static int init_images()
{
	back = image_load(IMG_DIR "background.png");
	return 0;
}

static void draw_image(SDL_Surface *img, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	SDL_BlitSurface(img, NULL, screen, &dest);
}

static void draw_background()
{
	draw_image(back, 0, 0);
}

static void draw_scene(struct sprite *cow_black)
{
	draw_background();
	draw(cow_black);
	SDL_Flip(screen);
}
