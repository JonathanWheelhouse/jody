#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL_image.h>
#include <time.h>
#include "gamedefs.h"
#include "resources.h"
#include "img_dir.h"

SDL_Cursor *cursor_arrow;
SDL_Cursor *cursor_wheelhouse;
SDL_Cursor *cursor_wheelhouse_transparent;
SDL_Cursor *cursor_wheelhouse_inverted;
SDL_Cursor *cursor_wheelhouse_black_with_white_lines;

SDL_Cursor *create_cursor_arrow(void)
{
	 /* Stolen from the mailing list */
	 /* Creates a new mouse cursor from an XPM */


	 /* XPM */
	 static const char *arrow[] = {
		  /* width height num_colors chars_per_pixel */
		  "    32    32        3            1",
		  /* colors */
		  "X c #000000",
		  ". c #ffffff",
		  "  c None",
		  /* pixels */
		  "X                               ",
		  "XX                              ",
		  "X.X                             ",
		  "X..X                            ",
		  "X...X                           ",
		  "X....X                          ",
		  "X.....X                         ",
		  "X......X                        ",
		  "X.......X                       ",
		  "X........X                      ",
		  "X.....XXXXX                     ",
		  "X..X..X                         ",
		  "X.X X..X                        ",
		  "XX  X..X                        ",
		  "X    X..X                       ",
		  "     X..X                       ",
		  "      X..X                      ",
		  "      X..X                      ",
		  "       XX                       ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "                                ",
		  "0,0"
	 };

	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (arrow[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
					mask[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(arrow[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

static const char *wheelhouse[] = {
	 /* width height num_colors chars_per_pixel */
	 "    32    32        3            1",
	 /* colors */
	 "X c #000000",
	 ". c #ffffff",
	 "  c None",
	 /* pixels */
	 "       X                X       ",
	 "     X.X.X            X...X     ",
	 "   X...X...X         X.....X    ",
	 " X.....X.....X      X.......X   ",
	 "X......X......X    X.........X  ",
	 "X......X......X   X...........X ",
	 "XXXXXXXXXXXXXXX  XXXXXXXXXXXXXXX",
	 "X......X......X  X.............X",
	 "X......X......X  X.............X",
	 " X.....X.....X   X.............X",
	 "   X...X...X     X.............X",
	 "     X.X.X       X.............X",
	 "       X         XXXXXXXXXXXXXXX",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "                                ",
	 "0,0"
};

SDL_Cursor *create_cursor_wheelhouse(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
					mask[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
SDL_Cursor *create_cursor_wheelhouse_transparent(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
/*
 * Make it transparent rather than white.
 */
/* 				mask[i] |= 0x01; */
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
SDL_Cursor *create_cursor_wheelhouse_inverted(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					data[i] |= 0x01;
					mask[i] |= 0x01;
					break;
			   case '.':
					data[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
SDL_Cursor *create_cursor_wheelhouse_black_with_white_lines(void)
{
	 int i, row, col;
	 Uint8 data[4*32];
	 Uint8 mask[4*32];
	 int hot_x, hot_y;

	 i = -1;
	 for (row=0; row<32; ++row) {
		  for (col=0; col<32; ++col) {
			   if ( col % 8 ) {
					data[i] <<= 1;
					mask[i] <<= 1;
			   } else {
					++i;
					data[i] = mask[i] = 0;
			   }
			   switch (wheelhouse[4+row][col]) {
			   case 'X':
					mask[i] |= 0x01;
					break;
			   case '.':
					data[i] |= 0x01;
					break;
			   case ' ':
					break;
			   }
		  }
	 }
	 sscanf(wheelhouse[4+row], "%d,%d", &hot_x, &hot_y);
	 return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
