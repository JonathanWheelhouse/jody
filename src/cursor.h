#ifndef CURSOR_H
#define CURSOR_H

#include <SDL/SDL.h>

struct cursors
{
	SDL_Cursor *cursor_arrow;
	SDL_Cursor *cursor_wheelhouse;
	SDL_Cursor *cursor_wheelhouse_transparent;
	SDL_Cursor *cursor_wheelhouse_inverted;
	SDL_Cursor *cursor_wheelhouse_black_with_white_lines;
};

SDL_Cursor *create_cursor_arrow(void);
SDL_Cursor *create_wheelhouse_cursor(void);
SDL_Cursor *create_wheelhouse_cursor_transparent(void);
SDL_Cursor *create_wheelhouse_cursor_inverted(void);
SDL_Cursor *create_wheelhouse_cursor_black_with_white_lines(void);

struct cursors *setup_cursors(void);

#endif
