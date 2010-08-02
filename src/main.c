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

/* TODO no global variables
   Have a type in each file?
   Then use that type to decide if it can be branded?
   Or only be able to brand cows.
*/

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

struct brands
{
	SDL_Surface *brand_wheelhouse;
	SDL_Surface *brand_wheelhouse_transparent;
	SDL_Surface *brand_wheelhouse_inverted;
	SDL_Surface *brand_wheelhouse_black_with_white_lines;
};

/* Prototypes */
static void init_sdl(bool fullscreen);
static void handle_args(int argc, char *argv[], bool *fullscreen);
static void print_help(void);
static void print_usage(void);
static SDL_Surface *get_background();
static struct cursors setup_cursors(void);
static struct brands setup_brands(void);
static void close_sprites(struct sprite **sprites);
static void play_game(SDL_Surface *screen, SDL_Surface *back, struct sprite **sprites, struct cursors *cursors, struct brands *brands);
static void handle_events(SDL_Surface *screen, int *quit, int *pause, bool *mouse_clicked, int *x_mouse, int *y_mouse, struct cursors *cursors, struct brands *brands, SDL_Surface **brand);
static void move(struct sprite *sprites[], int *pause, double elapsed_ticks);
static Uint32 get_elapsed_ticks(void);
static void main_draw(struct sprite **sprites, SDL_Surface *brand);
static void check_branded(SDL_Rect *src, struct sprite **sprites, int x_mouse, int y_mouse);

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
	struct brands brands = setup_brands();

	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_Surface *background = get_background();

	struct sprite **sprites = setup_img(screen);

	play_game(screen, background, sprites, &cursors, &brands);

	close_sprites(sprites);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(background);
	/* SDL_FreeSurface(brand); */

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

	cursors.cursor_wheelhouse = create_wheelhouse_cursor();
	if (!cursors.cursor_wheelhouse) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors.cursor_wheelhouse_transparent = create_wheelhouse_cursor_transparent();
	if (!cursors.cursor_wheelhouse_transparent) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors.cursor_wheelhouse_inverted = create_wheelhouse_cursor_inverted();
	if (!cursors.cursor_wheelhouse_inverted) {
		printf("Unable to create_wheelhouse_cursor_inverted\n");
		exit(EXIT_FAILURE);
	}

	cursors.cursor_wheelhouse_black_with_white_lines = create_wheelhouse_cursor_black_with_white_lines();
	if (!cursors.cursor_wheelhouse_black_with_white_lines) {
		printf("Unable to create_wheelhouse_cursor_black_with_white_lines\n");
		exit(EXIT_FAILURE);
	}

	return cursors;
}

static struct brands setup_brands(void)
{
	struct brands brands;

	brands.brand_wheelhouse = create_wheelhouse_image();
	if (!brands.brand_wheelhouse) {
		printf("Unable to create brand_wheelhouse\n");
		exit(EXIT_FAILURE);
	}

	brands.brand_wheelhouse_transparent = load_image(IMG_DIR "wheelhouse_image_transparent.png");
	if (!brands.brand_wheelhouse_transparent) {
		printf("Unable to load brand_wheelhouse_transparent\n");
		exit(EXIT_FAILURE);
	}

	brands.brand_wheelhouse_inverted = load_image(IMG_DIR "wheelhouse_image_inverted.png");
	if (!brands.brand_wheelhouse_inverted) {
		printf("Unable to load brand_wheelhouse_inverted\n");
		exit(EXIT_FAILURE);
	}

	brands.brand_wheelhouse_black_with_white_lines = load_image(IMG_DIR "wheelhouse_image_black_with_white_lines.png");
	if (!brands.brand_wheelhouse_black_with_white_lines) {
		printf("Unable to brand_wheelhouse_black_with_white_lines\n");
		exit(EXIT_FAILURE);
	}

	return brands;
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

static void play_game(SDL_Surface *screen, SDL_Surface *back, struct sprite **sprites, struct cursors *cursors, struct brands *brands)
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
	bool mouse_clicked;
	int x_mouse = 0;
	int y_mouse = 0;
	SDL_Surface *brand = NULL;
	while (!quit) {
		mouse_clicked = false;
		handle_events(screen, &quit, &pause, &mouse_clicked, &x_mouse, &y_mouse, cursors, brands, &brand);

		/* ai logic */
		/* ... check branded */
		if (mouse_clicked)
			check_branded(&src, sprites, x_mouse, y_mouse);

		/* ... handle collisions */

		/* ... set statuses */

		/* ... moves */
		Uint32 elapsed_ticks = get_elapsed_ticks();
		move(sprites, &pause, elapsed_ticks);

		// render
		draw_background(back, screen);
		main_draw(sprites, brand);

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

static void handle_events(SDL_Surface *screen, int *quit, int *pause, bool *mouse_clicked, int *x_mouse, int *y_mouse, struct cursors *cursors, struct brands *brands, SDL_Surface **brand)
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
				*brand = brands->brand_wheelhouse;
				break;
			case SDLK_3:
				SDL_SetCursor(cursors->cursor_wheelhouse_transparent);
				*brand = brands->brand_wheelhouse_transparent;
				break;
			case SDLK_4:
				SDL_SetCursor(cursors->cursor_wheelhouse_inverted);
				*brand = brands->brand_wheelhouse_inverted;
				break;
			case SDLK_5:
				SDL_SetCursor(cursors->cursor_wheelhouse_black_with_white_lines);
				*brand = brands->brand_wheelhouse_black_with_white_lines;
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
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) { 
				*mouse_clicked = true;
				*x_mouse = event.button.x;
				*y_mouse = event.button.y; 
			}
		}
	}
}

static void check_branded(SDL_Rect *src, struct sprite **sprites, int x_mouse, int y_mouse)
{
	for (int i = 0; i < NUM_IMAGES; i++) {
		double x_rhs = sprites[i]->x + src->w;
		double y_bot = sprites[i]->y + src->h;

		if (sprites[i]->sprite_base->can_be_branded && !sprites[i]->is_branded)
			if (x_mouse >= sprites[i]->x && x_mouse <= x_rhs && y_mouse >= sprites[i]->y && y_mouse <= y_bot)
				sprites[i]->is_branded = true;
	}
}

static void move(struct sprite *sprites[], int *pause, double elapsed_ticks)
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

static void main_draw(struct sprite **sprites, SDL_Surface *brand)
{
	for (int i = 0; i < NUM_IMAGES; i++) {
		draw(sprites[i], brand);
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
