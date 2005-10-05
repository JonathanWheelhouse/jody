#ifndef RESOURCES_H
#define RESOURCES_H

#include <SDL/SDL.h>

extern SDL_Surface *screen;


enum image_idx {
	 IMG_BACKGROUND,
	 IMG_CLOUD,
	 IMG_COW_BLACK_TILE,
	 IMG_COW_BLACK_LEFT,
	 IMG_COW_BLACK,
	 IMG_COW_BROWN_LEFT,
	 IMG_COW_BROWN,
	 IMG_COW_ORANGE_LEFT,
	 IMG_COW_ORANGE,
	 IMG_COW_RED_LEFT,
	 IMG_COW_RED,
	 IMG_COW_WHITE_LEFT,
	 IMG_COW_WHITE,
	 IMG_COW_YELLOW_LEFT,
	 IMG_COW_YELLOW,
	 IMG_COW_YELLOW_WITH_RED_OUTLINE_LEFT,
	 IMG_COW_YELLOW_WITH_RED_OUTLINE,
	 IMG_KANGAROO_BLACK_LEFT,
	 IMG_KANGAROO_BLACK,
	 IMG_KANGAROO_GREY_LEFT,
	 IMG_KANGAROO_GREY,
	 IMG_KANGAROO_RED_LEFT,
	 IMG_KANGAROO_RED,
	 NUM_IMAGES
};


struct image {
	 SDL_Surface *surface;
	 int x;
	 int y;
};

extern struct image images[NUM_IMAGES];

extern void setup(int fullscreen);

extern SDL_Cursor *create_cursor_arrow(void);
extern SDL_Cursor *create_cursor_wheelhouse(void);
extern SDL_Cursor *create_cursor_wheelhouse_transparent(void);
extern SDL_Cursor *create_cursor_wheelhouse_inverted(void);
extern SDL_Cursor *create_cursor_wheelhouse_black_with_white_lines(void);

extern SDL_Cursor *cursor_arrow;
extern SDL_Cursor *cursor_wheelhouse;
extern SDL_Cursor *cursor_wheelhouse_transparent;
extern SDL_Cursor *cursor_wheelhouse_inverted;
extern SDL_Cursor *cursor_wheelhouse_black_with_white_lines;

#endif
