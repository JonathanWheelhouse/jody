#ifndef CURSOR_H
#define CURSOR_H

#include <SDL/SDL.h>

extern SDL_Cursor *create_cursor_arrow(void);
extern SDL_Cursor *create_cursor_wheelhouse(void);
extern SDL_Cursor *create_cursor_wheelhouse_transparent(void);
extern SDL_Cursor *create_cursor_wheelhouse_inverted(void);
extern SDL_Cursor *create_cursor_wheelhouse_black_with_white_lines(void);

#endif
