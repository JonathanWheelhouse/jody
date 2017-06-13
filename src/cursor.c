#include <stdio.h>
#include <string.h>
#include <SDL_image.h>
#include <time.h>
#include "util.h"
#include "gamedefs.h"
#include "datadir.h"
#include "wheelhouse.h"

#include "cursor.h"

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


SDL_Cursor *create_wheelhouse_cursor(void)
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


SDL_Cursor *create_wheelhouse_cursor_transparent(void)
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

SDL_Cursor *create_wheelhouse_cursor_inverted(void)
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
SDL_Cursor *create_wheelhouse_cursor_black_with_white_lines(void)
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

struct cursors *setup_cursors(void)
{
	struct cursors *cursors = xmalloc(sizeof(struct cursors));

	cursors->cursor_arrow = create_cursor_arrow();
	if (!cursors->cursor_arrow) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors->cursor_wheelhouse = create_wheelhouse_cursor();
	if (!cursors->cursor_wheelhouse) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors->cursor_wheelhouse_transparent = create_wheelhouse_cursor_transparent();
	if (!cursors->cursor_wheelhouse_transparent) {
		printf("Unable to create_cursor_arrow\n");
		exit(EXIT_FAILURE);
	}

	cursors->cursor_wheelhouse_inverted = create_wheelhouse_cursor_inverted();
	if (!cursors->cursor_wheelhouse_inverted) {
		printf("Unable to create_wheelhouse_cursor_inverted\n");
		exit(EXIT_FAILURE);
	}

	cursors->cursor_wheelhouse_black_with_white_lines = create_wheelhouse_cursor_black_with_white_lines();
	if (!cursors->cursor_wheelhouse_black_with_white_lines) {
		printf("Unable to create_wheelhouse_cursor_black_with_white_lines\n");
		exit(EXIT_FAILURE);
	}

	return cursors;
}
