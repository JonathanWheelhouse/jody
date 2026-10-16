#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "util.h"
#include "wheelhouse.h"
#include "cursor.h"

static SDL_Cursor *
create_cursor_from_xpm(const char **xpm,
                       int hot_x,
                       int hot_y,
                       int invert_data_mask)
{
    enum {
        WIDTH = 32,
        HEIGHT = 32
    };

    SDL_Surface *surface =
        SDL_CreateSurface(WIDTH, HEIGHT, SDL_PIXELFORMAT_RGBA32);

    if (!surface) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "SDL_CreateSurface failed: %s",
            SDL_GetError()
        );
        return nullptr;
    }

    if (SDL_MUSTLOCK(surface) && !SDL_LockSurface(surface)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "SDL_LockSurface failed: %s",
            SDL_GetError()
        );
        SDL_DestroySurface(surface);
        return nullptr;
    }

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch_pixels = surface->pitch / (int)sizeof(*pixels);

    for (int y = 0; y < HEIGHT; ++y) {
        const char *row = xpm[y];

        if (!row) {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Cursor row %d is NULL",
                y
            );

            if (SDL_MUSTLOCK(surface)) {
                SDL_UnlockSurface(surface);
            }

            SDL_DestroySurface(surface);
            return nullptr;
        }

        size_t row_length = strlen(row);

        for (int x = 0; x < WIDTH; ++x) {
            char c = (x < (int)row_length) ? row[x] : ' ';

            Uint8 r = 0;
            Uint8 g = 0;
            Uint8 b = 0;
            Uint8 a = 0;

			switch (c) {
			case 'X':
				if (invert_data_mask == 1) {
					/* Inverted mode: X becomes White */
					r = 255; g = 255; b = 255; a = 255;
				} else if (invert_data_mask == 2) {
					/* Transparent mode: X becomes Transparent */
					a = 0;
				} else {
					/* Standard mode: X is Black */
					r = 0; g = 0; b = 0; a = 255;
				}
				break;

			case '.':
				if (invert_data_mask == 1) {
					/* Inverted mode: Dot becomes Black */
					r = 0; g = 0; b = 0; a = 255;
				} else {
					/* Standard mode: Dot is White */
					r = 255; g = 255; b = 255; a = 255;
				}
				break;

			case ' ':
			default:
				/* Transparent background */
				r = 0; g = 0; b = 0; a = 0;
				break;
			}

            pixels[y * pitch_pixels + x] = SDL_MapSurfaceRGBA(surface, r, g, b, a);
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    SDL_Cursor *cursor =
        SDL_CreateColorCursor(surface, hot_x, hot_y);

    if (!cursor) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "SDL_CreateColorCursor failed: %s",
            SDL_GetError()
        );
    }

    SDL_DestroySurface(surface);
    return cursor;
}


static SDL_Cursor *
create_cursor_arrow(void)
{
    /*
     * This array contains exactly 32 rows.
     *
     * Do not include the XPM header, colour definitions, or hotspot
     * in this array.
     */
    static const char *arrow_pixels[32] = {
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
        "                                "
    };

    return create_cursor_from_xpm(
        arrow_pixels,
        0,      /* hot_x */
        0,      /* hot_y */
        0
    );
}


struct cursors *
setup_cursors(void)
{
    struct cursors *cursors = xmalloc(sizeof(*cursors));

    cursors->cursor_arrow = create_cursor_arrow();

    /*
     * wheelhouse must also contain exactly 32 image rows
     * if it is passed to create_cursor_from_xpm().
     */
    cursors->cursor_wheelhouse =
        create_cursor_from_xpm((const char **)wheelhouse, 0, 0, 0);

	/* Mode 0: Standard (Black X, White .) */
    cursors->cursor_wheelhouse_transparent =
        create_cursor_from_xpm(
            (const char **)wheelhouse, 0, 0, 2);

	/* Mode 1: Inverted (White X, Black .) */
    cursors->cursor_wheelhouse_inverted =
        create_cursor_from_xpm(
            (const char **)wheelhouse, 0, 0, 1);

	/* Mode 0: Standard */
    cursors->cursor_wheelhouse_black_with_white_lines =
        create_cursor_from_xpm(
            (const char **)wheelhouse, 0, 0, 0);

    if (!cursors->cursor_arrow ||
        !cursors->cursor_wheelhouse ||
        !cursors->cursor_wheelhouse_transparent ||
        !cursors->cursor_wheelhouse_inverted ||
        !cursors->cursor_wheelhouse_black_with_white_lines) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to create one or more cursors: %s",
            SDL_GetError()
        );

        free_cursors(cursors);
        return nullptr;
    }

    return cursors;
}


void
free_cursors(struct cursors *cursors)
{
    if (!cursors) {
        return;
    }

    if (cursors->cursor_arrow) {
        SDL_DestroyCursor(cursors->cursor_arrow);
    }

    if (cursors->cursor_wheelhouse) {
        SDL_DestroyCursor(cursors->cursor_wheelhouse);
    }

    if (cursors->cursor_wheelhouse_transparent) {
        SDL_DestroyCursor(cursors->cursor_wheelhouse_transparent);
    }

    if (cursors->cursor_wheelhouse_inverted) {
        SDL_DestroyCursor(cursors->cursor_wheelhouse_inverted);
    }

    if (cursors->cursor_wheelhouse_black_with_white_lines) {
        SDL_DestroyCursor(
            cursors->cursor_wheelhouse_black_with_white_lines
        );
    }

    free(cursors);
}
