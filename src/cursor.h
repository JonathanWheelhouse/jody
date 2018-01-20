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

struct cursors *setup_cursors(void);

#endif
