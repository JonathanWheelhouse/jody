#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

#include "gamedefs.h"
#include "cursor.h"
#include "util.h"
#include "sprite.h"
#include "datadir.h"

/* constants */
#define X_DIST 10
#define Y_DIST 1

/* global variables */

struct gamestate
{
	bool fullscreen;

	struct engine *engine;

	/* Sprites */

	/* Global game state */
	int		running;
	int		level;
	int		lives;
	int		score;
	int		enemycount;

	/* Objects */

	/* Statistics */
	int		logic_frames;
	int		rendered_frames;
};

enum image_idx {
	 IMG_BACKGROUND,
	 IMG_CLOUD,
	 IMG_COW_BLACK,
	 IMG_COW_BROWN,
	 IMG_COW_ORANGE,
	 IMG_COW_RED,
	 IMG_COW_WHITE,
	 IMG_COW_YELLOW,
	 IMG_COW_YELLOW_WITH_RED_OUTLINE,
	 IMG_KANGAROO_BLACK,
	 IMG_KANGAROO_GREY,
	 IMG_KANGAROO_RED,
	 NUM_IMAGES
};

const char *image_names[NUM_IMAGES] = {
	 IMG_DIR "background",
	 IMG_DIR "cloud",
	 IMG_DIR "cow_black",
	 IMG_DIR "cow_brown",
	 IMG_DIR "cow_orange",
	 IMG_DIR "cow_red",
	 IMG_DIR "cow_white",
	 IMG_DIR "cow_yellow",
	 IMG_DIR "cow_yellow_with_red_outline",
	 IMG_DIR "kangaroo_black",
	 IMG_DIR "kangaroo_grey",
	 IMG_DIR "kangaroo_red"
};

struct engine
{
	SDL_Surface	*back, *screen;

	int quit, pause;

	/* Background graphics */
	struct map *map;
	
	/* Sprites and stuff */
	int		nsprites;
	struct sprite	**sprites;

	SDL_Cursor *cursor_arrow;
	SDL_Cursor *cursor_wheelhouse;
	SDL_Cursor *cursor_wheelhouse_transparent;
	SDL_Cursor *cursor_wheelhouse_inverted;
	SDL_Cursor *cursor_wheelhouse_black_with_white_lines;

};

/* Level map */
struct map
{
	struct engine *owner;

	int		w, h;					/* Size of map (tiles) */
	unsigned char	*map;			/* 2D aray of tile indices */
	unsigned char	*hit;			/* 2D aray of collision flags */

	int		tw, th;					/* Size of one tile (pixels) */
	SDL_Surface	*tiles;				/* Tile palette image */
	unsigned char	hitinfo[256];	/* Collision info for the tiles */
};

/* Prototypes */
static struct gamestate *init(int argc, char *argv[]);
static void close(struct gamestate *gamestate);

static bool is_fullscreen(int argc, char *argv[]);
static void print_help();
static void print_usage();
static struct engine *open_engine(SDL_Surface *screen);
static void close_engine(struct engine *engine);
static void play_game(struct engine *engine);
static void handle_events(struct engine *engine);
static void main_draw(SDL_Rect *src, struct engine *engine, double time_scale, double time_scale_factor);

static void set_icon(void);
static void setup_img(struct engine *engine);
static void load_img(int i, struct engine *engine, unsigned int x_scale, unsigned int y_scale);

static void draw_image(SDL_Surface *img, SDL_Surface *screen, int x, int y);
static void draw_background(SDL_Surface *back, SDL_Surface *screen);

int main(int argc, char *argv[])
{
	struct gamestate *gs = init(argc, argv);
	if(!gs)
		exit(EXIT_FAILURE);

	play_game(gs->engine);

	close(gs);

	exit(EXIT_SUCCESS);
}

static struct gamestate *init(int argc, char *argv[])
{
	srand(time(0));

	bool fullscreen = is_fullscreen(argc, argv);

	struct gamestate *gs = (struct gamestate *)calloc(1, sizeof(struct gamestate));
	if(!gs)
		return NULL;

	gs->fullscreen = fullscreen;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);

	SDL_WM_SetCaption("jody", "jody");
	set_icon();

	if (SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF |
						 (fullscreen ? SDL_FULLSCREEN : 0)) == NULL) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		free(gs);
		exit(1);
	}

	SDL_Surface *screen = SDL_GetVideoSurface();
	struct engine *engine = open_engine(screen);
	if (!engine)
		return NULL;

	setup_img(engine);

	gs->engine = engine;

	return gs;
}

static void close(struct gamestate *gs)
{
	close_engine(gs->engine);
	free(gs);
}

static bool is_fullscreen(int argc, char *argv[])
{
	bool fullscreen = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--fullscreen") == 0 || strcmp(argv[i], "-f") == 0) {
			fullscreen = true;
		} else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			print_help();
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i], "--usage") == 0 || strcmp(argv[i], "-u") == 0) {
			print_usage();
			exit(EXIT_SUCCESS);
		} else {
			print_usage();
			exit(EXIT_FAILURE);
		}
	}
	return fullscreen;
}

static struct engine *open_engine(SDL_Surface *screen)
{
	struct engine *engine = (struct engine *)calloc(1, sizeof(struct engine));
	if (!engine) {
		return NULL;
	}
	engine->screen = screen;

/* FIXME */
/* 	engine->map = NULL; */

	/* Setup the cursors. */
	engine->cursor_arrow = create_cursor_arrow();
	if (!engine->cursor_arrow) {
		close_engine(engine);
		return NULL;
	}
	engine->cursor_wheelhouse = create_cursor_wheelhouse();
	if (!engine->cursor_wheelhouse) {
		close_engine(engine);
		return NULL;
	}
	engine->cursor_wheelhouse_transparent = create_cursor_wheelhouse_transparent();
	if (!engine->cursor_wheelhouse_transparent) {
		close_engine(engine);
		return NULL;
	}
	engine->cursor_wheelhouse_inverted = create_cursor_wheelhouse_inverted();
	if (!engine->cursor_wheelhouse_inverted) {
		close_engine(engine);
		return NULL;
	}
	engine->cursor_wheelhouse_black_with_white_lines = create_cursor_wheelhouse_black_with_white_lines();
	if (!engine->cursor_wheelhouse_black_with_white_lines) {
		close_engine(engine);
		return NULL;
	}
	return engine;
}

static void close_engine(struct engine *engine)
{
	if(engine->screen)
		SDL_FreeSurface(engine->screen);

	/* No need to free the background; it's the first sprite */

	if(engine->map)
		free(engine->map);

	if (engine->sprites) {
		for (int i = 0; i < NUM_IMAGES; i++) {
			if (engine->sprites[i])
				free_sprite(engine->sprites[i]);
		}
		free(engine->sprites);
	}
	
	if (engine->cursor_arrow)
		SDL_FreeCursor(engine->cursor_arrow);
	if (engine->cursor_wheelhouse)
		SDL_FreeCursor(engine->cursor_wheelhouse);
	if (engine->cursor_wheelhouse_transparent)
		SDL_FreeCursor(engine->cursor_wheelhouse_transparent);
	if(engine->cursor_wheelhouse_inverted)
		SDL_FreeCursor(engine->cursor_wheelhouse_inverted);
	if(engine->cursor_wheelhouse_black_with_white_lines)
		SDL_FreeCursor(engine->cursor_wheelhouse_black_with_white_lines);
	
	free(engine);
}

void set_icon(void)
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

void setup_img(struct engine *engine)
{
	engine->sprites = (struct sprite **)calloc(NUM_IMAGES, sizeof(struct sprite *));
	if(!engine->sprites)
	{
		close_engine(engine);
		fprintf(stderr, "\nError: I couldn't allocate memory for engine-sprites.\n");
		exit(1);
	}

	srand (time(0));
	unsigned int x_scale = RAND_MAX / SCREEN_WIDTH;
	unsigned int y_scale = RAND_MAX / SCREEN_HEIGHT;

	for (int i = 0; i < NUM_IMAGES; i++)
		load_img(i, engine, x_scale, y_scale);

	/* first image = background */
	engine->back = engine->sprites[0]->sprite_base->frames[0]->image;
}

static void load_img(int i, struct engine *engine, unsigned int x_scale, unsigned int y_scale)
{
	struct sprite_base *base = base_init(image_names[i]);
	if (base == NULL) {
		fprintf(stderr,
				"\nError: I couldn't initialise:\n"
				"%s\n"
				"The Simple DirectMedia error that occurred was:\n"
				"%s\n\n", image_names[i], SDL_GetError());
		exit(1);
	}
	struct sprite *sprite = sprite_init(base, engine->screen);
	if (sprite == NULL) {
		fprintf(stderr,
				"\nError: I couldn't initialise the sprite:\n"
				"%s\n"
				"The Simple DirectMedia error that occurred was:\n"
				"%s\n\n", image_names[i], SDL_GetError());
		exit(1);
	}

	unsigned int x = random();
	x /= x_scale;
	unsigned int y = random();
	y /= y_scale;


	/* Put clouds in the sky; animals on the ground. */
	int half = SCREEN_HEIGHT / 2; 
	printf("half\t%d", half);
	printf("\timage_names[i]\t%s", image_names[i]);
	if (i == IMG_CLOUD) {
		printf("\tmatched!");
		if (y > half) ;
			y = base->image_height;
	} else 
		if (y < half)
			y = SCREEN_HEIGHT - base->image_height;

		
	printf("\ty\t%d\n", y);


	set(sprite, x, y);
	set_speed(sprite, 1);
	engine->sprites[i] = sprite;
}

static void play_game(struct engine *engine)
{

	/* keep track of frames and time */
	int prev_ticks = 0, cur_ticks = 0;
	prev_ticks = SDL_GetTicks();

	int start_time, end_time;
	start_time = time(NULL);
	int frames_drawn = 0;

	/* 126 = width of 1 cow of the 3 cow tile; 95 = height of cow */
	SDL_Rect src;
	src.w = 126;
	src.h = 95;
	src.x = 0;
	src.y = 0;

	engine->quit = engine->pause = 0;
	while (engine->quit == 0) {

		/* Determine how many milliseconds have passed since
		   the last frame, and update our motion scaling. */

		prev_ticks = cur_ticks;
		cur_ticks = SDL_GetTicks();

		double time_scale_factor = 50;
		double time_scale = (double) (cur_ticks - prev_ticks) / time_scale_factor;

		handle_events(engine);

		/* ai logic - calc moves */

		draw_background(engine->back, engine->screen);

		main_draw(&src, engine, time_scale, time_scale_factor);

		SDL_Flip(engine->screen);

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

static void print_help()
{
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
}

static void print_usage()
{
	printf("\nUsage: jody [--fullscreen] | [--help (-h) | --usage (-u)]\n\n"
		   " --fullscreen or -f - Display in full screen instead of a window, if possible.\n"
		   "\n");
}

static void handle_events(struct engine *engine)
{
	SDL_Event event;
	while (SDL_PollEvent(&event) > 0) {
		if (event.type == SDL_QUIT) {
			engine->quit = 1;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
			case SDLK_q:
				engine->quit = 1;
				break;
			case SDLK_1:
				SDL_SetCursor(engine->cursor_arrow);
				break;
			case SDLK_2:
				SDL_SetCursor(engine->cursor_wheelhouse);
				break;
			case SDLK_3:
				SDL_SetCursor(engine->cursor_wheelhouse_transparent);
				break;
			case SDLK_4:
				SDL_SetCursor(engine->cursor_wheelhouse_inverted);
				break;
			case SDLK_5:
				SDL_SetCursor(engine->cursor_wheelhouse_black_with_white_lines);
				break;
			case SDLK_f:
				SDL_WM_ToggleFullScreen(engine->screen);
				break;
			case SDLK_p:
			case SDLK_SPACE:
				engine->pause = !engine->pause;
				break;
			default:
				break;
			}
		}
	}
}

static void main_draw(SDL_Rect *src, struct engine *engine, double time_scale, double time_scale_factor)
{
	/* SDL_Rect dest; */

	/* background */
/* 	dest.x = 0; */
/* 	dest.y = 0; */
/* 	dest.w = (images[IMG_BACKGROUND].surface)->w; */
/* 	dest.h = (images[IMG_BACKGROUND].surface)->h; */
/* 	SDL_BlitSurface(images[IMG_BACKGROUND].surface, NULL, screen, &dest); */

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
/* 		struct image *image = &images[i]; */
/* 		dest.w = image->surface->w; */
/* 		dest.h = image->surface->h; */
/* 		if (engine->pause) { */
/* 			dest.x = image->x; */
/* 			dest.y = image->y; */
/* 		} else */
/* 			dest = calc_dest(image); */

/* 		SDL_BlitSurface(image->surface, NULL, engine->screen, &dest); */

		if (!engine->pause)
			xadd(engine->sprites[i], X_DIST * time_scale);
		draw(engine->sprites[i], time_scale_factor);
	}

}

/* static SDL_Rect calc_dest(struct image *image) */
/* { */
/* 	SDL_Rect dest; */

/* 	/\* Move the cow horizontally. *\/ */
/* 	image->x += X_DIST * time_scale; */
/* 	if (image->x > SCREEN_WIDTH) */
/* 		image->x = 0; */
/* 	dest.x = image->x; */

/* 	/\* Move the cow vertically and randomly. *\/ */
/* 	/\* Add check for top and bottom; do something sensible */
/* 	   like bounce off. *\/ */

/* 	unsigned int odd = random() % 2; */
/* 	int dist; */
/* 	if (odd) */
/* 		dist = Y_DIST; */
/* 	else */
/* 		dist = - Y_DIST; */

/* 	image->y += dist * time_scale; */
/* 	if (image->y > SCREEN_HEIGHT || image->y < 0) */
/* 		image->y = SCREEN_HEIGHT / 2; */
	 
/* 	dest.y = image->y; */

/* 	return dest; */
/* } */

/* static SDL_Surface *image_load(char *file) */
/* { */
/* 	SDL_Surface *temp1, *temp2; */
/* 	temp1 = IMG_Load(file); */
/* 	if (temp1 == NULL) { */
/* 		fprintf(stderr, */
/* 				"\nError: 'static SDL_Surface * image_load' function couldn't load a graphics file:\n" */
/* 				"%s\n" */
/* 				"The Simple DirectMedia error that occurred was:\n" */
/* 				"%s\n\n", file, SDL_GetError()); */
/* 		return NULL; */
/* 	} */
/* 	temp2 = SDL_DisplayFormat(temp1); */
/* 	SDL_FreeSurface(temp1); */
/* 	return temp2; */
/* } */

/* static SDL_Surface *init_images() */
/* { */
/* 	return image_load(IMG_DIR "background.png"); */
/* } */

static void draw_image(SDL_Surface *img, SDL_Surface *screen, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	SDL_BlitSurface(img, NULL, screen, &dest);
}

static void draw_background(SDL_Surface *back, SDL_Surface *screen)
{
	draw_image(back, screen, 0, 0);
}
