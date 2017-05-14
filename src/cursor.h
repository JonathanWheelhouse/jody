#ifndef CURSOR_H
#define CURSOR_H

#include <SDL/SDL.h>

SDL_Cursor *create_cursor_arrow(void);
SDL_Cursor *create_wheelhouse_cursor(void);
SDL_Surface *create_wheelhouse_image(void);
SDL_Cursor *create_wheelhouse_cursor_transparent(void);
SDL_Cursor *create_wheelhouse_cursor_inverted(void);
SDL_Cursor *create_wheelhouse_cursor_black_with_white_lines(void);

#endif
