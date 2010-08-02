#ifndef CURSOR_H
#define CURSOR_H

#include <SDL/SDL.h>

extern SDL_Cursor *create_cursor_arrow(void);
extern SDL_Cursor *create_wheelhouse_cursor(void);
extern SDL_Surface *create_wheelhouse_image(void);
extern SDL_Cursor *create_wheelhouse_cursor_transparent(void);
extern SDL_Cursor *create_wheelhouse_cursor_inverted(void);
extern SDL_Cursor *create_wheelhouse_cursor_black_with_white_lines(void);

#endif
