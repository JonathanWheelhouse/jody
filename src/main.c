#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>

#include "gamedefs.h"
#include "cursor.h"
#include "util.h"
#include "surface.h"
#include "sprite.h"
#include "datadir.h"

/* macros */
#define STREQ(a, b) (!strcmp((a),(b)))

/* constants */
#define SPRITE_PIXELS_PER_SECOND 200
#define NUM_MILLISECONDS 1000;
#define X_DIST 25
#define Y_DIST 1

/* global variables */

enum image_idx {
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

struct cursors
{
	SDL_Cursor *cursor_arrow;
	SDL_Cursor *cursor_wheelhouse;
	SDL_Cursor *cursor_wheelhouse_transparent;
	SDL_Cursor *cursor_wheelhouse_inverted;
	SDL_Cursor *cursor_wheelhouse_black_with_white_lines;
};

/* Prototypes */
static void init_sdl(bool fullscreen);
static void handle_args(int argc, char *argv[], bool *fullscreen);
static void print_help(void);
static void print_usage(void);
static SDL_Surface *get_background();
static struct cursors setup_cursors(void);
static void close_sprites(struct sprite **sprites);
static void play_game(SDL_Surface *back, SDL_Surface *screen, struct sprite **sprites, struct cursors *cursors);
static void handle_events(SDL_Surface *screen, int *quit, int *pause, struct cursors *cursors);
static void move(SDL_Rect *src, struct sprite *sprites[], int *pause, double elapsed_ticks);
static Uint32 get_elapsed_ticks(void);
static void main_draw(SDL_Rect *src, struct sprite **sprites);

static void set_icon(void);
static struct sprite **setup_img(SDL_Surface *screen);
static struct sprite *load_img(int i, SDL_Surface *screen, unsigned int x_scale, unsigned int y_scale);
static void draw_image(SDL_Surface *img, SDL_Surface *screen, int x, int y);
static void draw_background(SDL_Surface *back, SDL_Surface *screen);

int main(int argc, char *argv[])
{
	bool fullscreen;
	handle_args(argc, argv, &fullscreen);

	init_sdl(fullscreen);

	struct cursors cursors = setup_cursors();

	SDL_Surface *screen = SDL_GetVideoSurface();

	struct sprite **sprites = setup_img(screen);

	SDL_Surface *background = get_background();

	play_game(background, screen, sprites, &cursors);

	close_sprites(sprites);

	SDL_FreeSurface(screen);
	free(background);

	exit(EXIT_SUCCESS);
}

static void init_sdl(bool fullscreen)
{
	srand(time(0));

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);

	SDL_WM_SetCaption("jody", "jody");
	set_icon();

	if (SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | (fullscreen ? SDL_FULLSCREEN : 0)) == NULL) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		exit(1);
	}
}

static SDL_Surface *get_background(void)
{
	char background_dir[] = {IMG_DIR "background"};
	char *background_path;
	if (asprintf(&background_path, "%s/1.png", background_dir) < 0) {
		fprintf(stderr,
				"\nError: I could not load the background image: %s\n"
				"The Simple DirectMedia error that occurred was:\n"
				"%s\n\n", background_dir, SDL_GetError());
		exit(1);
	}
	SDL_Surface *background = load_image(background_path);
	return background;
}

static void handle_args(int argc, char *argv[], bool *fullscreen)
{
	*fullscreen = false;

	for (int i = 1; i < argc; i++) {
		if (STREQ(argv[i], "--fullscreen") || STREQ(argv[i], "-f")) {
			*fullscreen = true;
		} else if (STREQ(argv[i], "--help") || STREQ(argv[i], "-h")) {
			print_help();
			exit(EXIT_SUCCESS);
		} else if (STREQ(argv[i], "--usage") || STREQ(argv[i], "-u")) {
			print_usage();
			exit(EXIT_SUCCESS);
		} else {
			print_usage();
			exit(EXIT_FAILURE);
		}
	}
}

static struct cursors setup_cursors(void)
{
	struct cursors cursors;

	cursors.cursor_arrow = create_cursor_arrow();
	if (!cursors.cursor_arrow) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors.cursor_wheelhouse = create_cursor_wheelhouse();
	if (!cursors.cursor_wheelhouse) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors.cursor_wheelhouse_transparent = create_cursor_wheelhouse_transparent();
	if (!cursors.cursor_wheelhouse_transparent) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors.cursor_wheelhouse_inverted = create_cursor_wheelhouse_inverted();
	if (!cursors.cursor_wheelhouse_inverted) {
		printf("Unable to create_cursor_wheelhouse_inverted\n");
		exit(EXIT_FAILURE);
	}

	cursors.cursor_wheelhouse_black_with_white_lines = create_cursor_wheelhouse_black_with_white_lines();
	if (!cursors.cursor_wheelhouse_black_with_white_lines) {
		printf("Unable to create_cursor_wheelhouse_black_with_white_lines\n");
		exit(EXIT_FAILURE);
	}

	return cursors;
}

static void close_sprites(struct sprite **sprites)
{
	for (int i = 0; i < NUM_IMAGES; i++) {
		if (sprites[i])
			free_sprite(sprites[i]);
	}
	free(sprites);
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
  
	SDL_WM_SetIcon(icon, NULL);

	SDL_FreeSurface(icon);
}

static struct sprite **setup_img(SDL_Surface *screen)
{
	struct sprite **sprites = xcalloc(NUM_IMAGES, sizeof(struct sprite *));

	srand (time(0));
	unsigned int x_scale = RAND_MAX / SCREEN_WIDTH;
	unsigned int y_scale = RAND_MAX / SCREEN_HEIGHT;

	for (int i = 0; i < NUM_IMAGES; i++) {
		sprites[i] = load_img(i, screen, x_scale, y_scale);
	}
	return sprites;
}

static struct sprite *load_img(int i, SDL_Surface *screen, unsigned int x_scale, unsigned int y_scale)
{
	printf("image_names[i] = [%s]\n", image_names[i]);
	struct sprite_base *base = base_init(image_names[i]);
	if (base == NULL) {
		fprintf(stderr,
				"\nError: I couldn't initialise:\n"
				"%s\n"
				"The Simple DirectMedia error that occurred was:\n"
				"%s\n\n", image_names[i], SDL_GetError());
		exit(1);
	}
	struct sprite *sprite = sprite_init(base, screen);
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
	/* printf("half\t%d", half); */
	/* printf("\timage_names[i]\t%s", image_names[i]); */
	if (i == IMG_CLOUD) {
		printf("\tmatched!");
		if (y > half)
			y = base->image_height;
	} else 
		if (y < half)
			y = SCREEN_HEIGHT - base->image_height;

	printf("\ty\t%d\n", y);

	set(sprite, x, y);
	set_speed(sprite, 1);
	return sprite;
}

static void play_game(SDL_Surface *back, SDL_Surface *screen, struct sprite **sprites, struct cursors *cursors)
{

	/* keep track of frames and time */
	int start_time, end_time;
	start_time = time(NULL);
	int frames_drawn = 0;

	/* 126 = width of 1 cow of the 3 cow tile; 95 = height of cow */
	SDL_Rect src;
	src.w = 126;
	src.h = 95;
	src.x = 0;
	src.y = 0;

	/* game loop logic: events, logic and rendering */
	int quit = false;
	int pause = false;
	while (!quit) {
		handle_events(screen, &quit, &pause, cursors);

		/* ai logic - calc moves */
		/* Determine how many milliseconds have passed since
		   the last frame, and update our motion scaling. */

		Uint32 elapsed_ticks = get_elapsed_ticks();
		move(&src, sprites, &pause, elapsed_ticks);

		// render
		draw_background(back, screen);
		main_draw(&src, sprites);

		frames_drawn++;

		if (SDL_Flip(screen) == -1)
			quit = true;
	}

	end_time = time(NULL);
	if (start_time == end_time) end_time++;

	/* Display the average framerate. */
	printf("Drew %i frames in %i seconds, for a framerate of %.2f fps.\n",
		frames_drawn, end_time-start_time, (float)frames_drawn/(float)(end_time-start_time));
}

static void print_help(void)
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

static void print_usage(void)
{
	printf("\nUsage: jody [--fullscreen] | [--help (-h) | --usage (-u)]\n\n"
		   " --fullscreen or -f - Display in full screen instead of a window, if possible.\n"
		   "\n");
}

static void handle_events(SDL_Surface *screen, int *quit, int *pause, struct cursors *cursors)
{
	SDL_Event event;
	while (SDL_PollEvent(&event) > 0) {
		if (event.type == SDL_QUIT) {
			*quit = true;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
			case SDLK_q:
				*quit = true;
				break;
			case SDLK_1:
				SDL_SetCursor(cursors->cursor_arrow);
				break;
			case SDLK_2:
				SDL_SetCursor(cursors->cursor_wheelhouse);
				break;
			case SDLK_3:
				SDL_SetCursor(cursors->cursor_wheelhouse_transparent);
				break;
			case SDLK_4:
				SDL_SetCursor(cursors->cursor_wheelhouse_inverted);
				break;
			case SDLK_5:
				SDL_SetCursor(cursors->cursor_wheelhouse_black_with_white_lines);
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

static void move(SDL_Rect *src, struct sprite *sprites[], int *pause, double elapsed_ticks)
{
	for (int i = 0; i < NUM_IMAGES; i++) {
		if (!*pause) {
			double elapsed_seconds = elapsed_ticks / NUM_MILLISECONDS;
			double distance = SPRITE_PIXELS_PER_SECOND * elapsed_seconds;
			xadd(sprites[i], distance);
		}
	}
}

static Uint32 get_elapsed_ticks(void)
{
	static Uint32 prev_ticks = 0;
	static Uint32 cur_ticks = 0;
	prev_ticks = cur_ticks;
	cur_ticks = SDL_GetTicks();
	Uint32 elapsed_ticks = cur_ticks - prev_ticks;
	return elapsed_ticks;
}

static void main_draw(SDL_Rect *src, struct sprite	**sprites)
{
	for (int i = 0; i < NUM_IMAGES; i++) {
		draw(sprites[i]);
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
