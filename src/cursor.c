#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "cursor.h"
#include "util.h"
#include "wheelhouse.h"

/* Load a PNG file directly into an SDL_Surface and convert it into a hardware cursor */
static SDL_Cursor *create_cursor_from_png(const char *filename, int hot_x, int hot_y)
{
    char *path = jody_image_path(filename);
    if (!path)
        return nullptr;

    SDL_Surface *surface = IMG_Load(path);
    free(path);

    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't load surface for cursor '%s': %s",
                     filename, SDL_GetError());
        return nullptr;
    }

    SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hot_x, hot_y);
    SDL_DestroySurface(surface); /* Surface can be freed as soon as cursor is created */

    if (!cursor) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL_CreateColorCursor failed for '%s': %s",
                     filename, SDL_GetError());
    }

    return cursor;
}

/* Generate cursor from raw XPM pixel array (solid white fill) */
static SDL_Cursor *create_cursor_from_xpm(const char **xpm, int hot_x, int hot_y)
{
    SDL_Surface *surface = SDL_CreateSurface(32, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) return nullptr;

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch_pixels = surface->pitch / (int)sizeof(*pixels);

    for (int y = 0; y < 32; ++y) {
        const char *row = xpm[y];
        size_t row_length = row ? strlen(row) : 0;

        for (int x = 0; x < 32; ++x) {
            char c = (x < (int)row_length) ? row[x] : ' ';
            Uint8 r = 0, g = 0, b = 0, a = 0;

            switch (c) {
            case 'X':
                a = 255; /* Black line */
                break;
            case '.':
                r = 255; g = 255; b = 255; a = 255; /* Solid white interior */
                break;
            case ' ':
            default:
                a = 0; /* Transparent */
                break;
            }

            pixels[y * pitch_pixels + x] = SDL_MapSurfaceRGBA(surface, r, g, b, a);
        }
    }

    SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hot_x, hot_y);
    SDL_DestroySurface(surface);
    return cursor;
}

/* Fallback/Default Arrow cursor built from XPM */
static SDL_Cursor *create_cursor_arrow(void)
{
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

    SDL_Surface *surface = SDL_CreateSurface(32, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) return nullptr;

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch_pixels = surface->pitch / (int)sizeof(*pixels);

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 32; ++x) {
            char c = arrow_pixels[y][x];
            Uint8 r = 0, g = 0, b = 0, a = 0;

            if (c == 'X') {
                a = 255;
            } else if (c == '.') {
                r = 255; g = 255; b = 255; a = 255;
            }

            pixels[y * pitch_pixels + x] = SDL_MapSurfaceRGBA(surface, r, g, b, a);
        }
    }

    SDL_Cursor *cursor = SDL_CreateColorCursor(surface, 0, 0);
    SDL_DestroySurface(surface);
    return cursor;
}

struct cursors *setup_cursors(void)
{
    struct cursors *cursors = xmalloc(sizeof(*cursors));

    cursors->cursor_arrow = create_cursor_arrow();

	/* Standard XPM cursor (solid white fill) */
    cursors->cursor_wheelhouse = create_cursor_from_xpm((const char **)wheelhouse, 0, 0);

	/* Transparent interior PNG cursor */
    cursors->cursor_wheelhouse_transparent =
        create_cursor_from_png("wheelhouse_image_transparent.png", 0, 0);

	/* Inverted silhouette PNG cursor */
    cursors->cursor_wheelhouse_inverted =
        create_cursor_from_png("wheelhouse_image_inverted.png", 0, 0);

	/* Black with white lines PNG cursor */
    cursors->cursor_wheelhouse_black_with_white_lines =
        create_cursor_from_png("wheelhouse_image_black_with_white_lines.png", 0, 0);

    if (!cursors->cursor_arrow ||
        !cursors->cursor_wheelhouse ||
        !cursors->cursor_wheelhouse_transparent ||
        !cursors->cursor_wheelhouse_inverted ||
        !cursors->cursor_wheelhouse_black_with_white_lines) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Unable to create one or more cursors\n");
        free_cursors(cursors);
        return nullptr;
    }

    return cursors;
}

void free_cursors(struct cursors *cursors)
{
    if (!cursors) return;

    if (cursors->cursor_arrow) SDL_DestroyCursor(cursors->cursor_arrow);
    if (cursors->cursor_wheelhouse) SDL_DestroyCursor(cursors->cursor_wheelhouse);
    if (cursors->cursor_wheelhouse_transparent) SDL_DestroyCursor(cursors->cursor_wheelhouse_transparent);
    if (cursors->cursor_wheelhouse_inverted) SDL_DestroyCursor(cursors->cursor_wheelhouse_inverted);
    if (cursors->cursor_wheelhouse_black_with_white_lines) SDL_DestroyCursor(cursors->cursor_wheelhouse_black_with_white_lines);

    free(cursors);
}
