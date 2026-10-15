#define _GNU_SOURCE

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "brands.h"
#include "cursor.h"
#include "datadir.h"
#include "gamedefs.h"
#include "sprite.h"
#include "surface.h"
#include "util.h"

/* macros  & helper functions */
static inline bool streq(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
}

/* constants */
constexpr double SPRITE_PIXELS_PER_SECOND = 200.0;
#define NUM_MILLISECONDS 1000.0
#define X_DIST 25
#define Y_DIST 1

enum image_idx {
    IMG_CLOUD,
    IMG_COW_BLACK,
    IMG_COW_BROWN,
    IMG_COW_ORANGE,
    IMG_COW_RED,
    IMG_COW_WHITE,
    IMG_COW_YELLOW,
    IMG_COW_YELLOW_WITH_RED_OUTLINE,
    IMG_KANGAROO_BLACK,
    IMG_KANGAROO_GREY,
    IMG_KANGAROO_RED,
    NUM_IMAGES
};

const char *image_names[NUM_IMAGES] = {
    IMG_DIR "cloud",
    IMG_DIR "cow_black",
    IMG_DIR "cow_brown",
    IMG_DIR "cow_orange",
    IMG_DIR "cow_red",
    IMG_DIR "cow_white",
    IMG_DIR "cow_yellow",
    IMG_DIR "cow_yellow_with_red_outline",
    IMG_DIR "kangaroo_black",
    IMG_DIR "kangaroo_grey",
    IMG_DIR "kangaroo_red"
};

/* Prototypes */
static void handle_args(int argc, char *argv[]);
static void print_help(void);
static void print_usage(void);
static SDL_Texture *get_background(SDL_Renderer *renderer);
static void handle_events(int *quit, int *pause, bool *mouse_clicked,
                          int *x_mouse, int *y_mouse,
                          struct cursors *cursors, struct brands *brands,
                          SDL_Texture **brand);
static void move(struct sprite *sprites[], int *pause, double elapsed_ticks);
static Uint32 get_elapsed_ticks(void);
static void main_draw(SDL_Renderer *renderer, struct sprite **sprites,
                      SDL_Texture *brand);
static void check_branded(struct sprite **sprites, int x_mouse, int y_mouse);

static struct sprite **setup_img(SDL_Renderer *renderer);
static struct sprite *load_img(SDL_Renderer *renderer, int i,
                               unsigned int x_scale, unsigned int y_scale);
static void free_sprites(struct sprite **sprites, int count);

int main(int argc, char *argv[])
{
    handle_args(argc, argv);

    SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    srand((unsigned int)time(nullptr));

    int window_width = SCREEN_WIDTH * 2;
    int window_height = SCREEN_HEIGHT * 2;

    SDL_Window *window = SDL_CreateWindow(
        "jody", window_width, window_height, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 2;
    }

    SDL_SetRenderLogicalPresentation(
        renderer,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_LOGICAL_PRESENTATION_LETTERBOX
    );

    /* Enable VSYNC via renderer property (SDL3 way) */
    SDL_SetRenderVSync(renderer, 1);

    struct cursors *cursors = setup_cursors();
    if (!cursors) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    struct brands *brands = setup_brands(renderer);
    if (!brands) {
        free_cursors(cursors);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 4;
    }

    SDL_Texture *background = get_background(renderer);
    if (!background) {
        cleanup_brands(brands);
        free_cursors(cursors);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 5;
    }

    /* Show splash screen for ~3 seconds */
    for (int i = 0; i < 3; ++i) {
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, background, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000);
    }

    struct sprite **sprites = setup_img(renderer);
    if (!sprites) {
        SDL_DestroyTexture(background);
        cleanup_brands(brands);
        free_cursors(cursors);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 6;
    }

    /* Game loop */
    int quit = 0;
    int pause = 0;
    bool mouse_clicked;
    int x_mouse = 0, y_mouse = 0;
    SDL_Texture *brand = nullptr;
    int frames_drawn = 0;
    Uint64 start_time = SDL_GetTicks();

    /* Ensure first call to get_elapsed_ticks returns 0, not total uptime */
    (void)get_elapsed_ticks();

    while (!quit) {
        mouse_clicked = false;
        handle_events(&quit, &pause, &mouse_clicked, &x_mouse, &y_mouse,
                      cursors, brands, &brand);

        if (mouse_clicked)
            check_branded(sprites, x_mouse, y_mouse);

        Uint32 elapsed_ticks = get_elapsed_ticks();
        move(sprites, &pause, elapsed_ticks);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, background, nullptr, nullptr);
        main_draw(renderer, sprites, brand);
        SDL_RenderPresent(renderer);

        frames_drawn++;
    }

    Uint64 end_time = SDL_GetTicks();
    if (end_time == start_time) end_time++;

    printf("Drew %i frames in %.1f seconds, for a framerate of %.2f fps.\n",
           frames_drawn,
           (double)(end_time - start_time) / 1000.0,
           (double)frames_drawn / ((double)(end_time - start_time) / 1000.0));

    /* Cleanup */
    free_sprites(sprites, NUM_IMAGES);
    SDL_DestroyTexture(background);
    cleanup_brands(brands);
    free_cursors(cursors);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

static SDL_Texture *get_background(SDL_Renderer *renderer)
{
    const char background_dir[] = IMG_DIR "background";
    char *background_path = nullptr;

    if (asprintf(&background_path, "%s/1.png", background_dir) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Could not allocate background path\n");
        return nullptr;
    }

    SDL_Texture *background = load_image(renderer, background_path);
    free(background_path); /* Fixed: leak */
    return background;
}

static void handle_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if (streq(argv[i], "--help") || streq(argv[i], "-h")) {
            print_help();
            exit(EXIT_SUCCESS);
        } else if (streq(argv[i], "--usage") || streq(argv[i], "-u")) {
            print_usage();
            exit(EXIT_SUCCESS);
        } else {
            print_usage();
            exit(EXIT_FAILURE);
        }
    }
}

static void print_help(void)
{
    printf(
        "\njody\n"
        "Version %s\n"
        "Copyright 2006 Jonathan Wheelhouse\n"
        "\n"
        "Game controls:\n"
        "  Keys:\n"
        "    ESC or q       - quit\n"
        "    SPACE or p     - toggle pause\n"
        "    arrow keys     - move person\n"
        "    1              - arrow cursor\n"
        "    2              - wheelhouse cursor\n"
        "    3              - transparent wheelhouse cursor\n"
        "    4              - inverted wheelhouse cursor\n"
        "    5              - black with white lines wheelhouse cursor\n"
        "  Mouse Movement   - Move branding iron.\n"
        "  Any Mouse Button - Brand!\n"
        "\n"
        "Run with \"--usage or -u\" for command-line options...\n"
        "\n", VERSION);
}

static void print_usage(void)
{
    printf("\nUsage: jody [--help (-h) | --usage (-u)]\n\n");
}

static void handle_events(int *quit, int *pause, bool *mouse_clicked,
                          int *x_mouse, int *y_mouse,
                          struct cursors *cursors, struct brands *brands,
                          SDL_Texture **brand)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            *quit = 1;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
            case SDLK_ESCAPE:
            case SDLK_Q:
                *quit = 1;
                break;
            case SDLK_1:
                SDL_SetCursor(cursors->cursor_arrow);
                break;
            case SDLK_2:
                SDL_SetCursor(cursors->cursor_wheelhouse);
                *brand = brands->brand_wheelhouse;
                break;
            case SDLK_3:
                SDL_SetCursor(cursors->cursor_wheelhouse_transparent);
                *brand = brands->brand_wheelhouse_transparent;
                break;
            case SDLK_4:
                SDL_SetCursor(cursors->cursor_wheelhouse_inverted);
                *brand = brands->brand_wheelhouse_inverted;
                break;
            case SDLK_5:
                SDL_SetCursor(cursors->cursor_wheelhouse_black_with_white_lines);
                *brand = brands->brand_wheelhouse_black_with_white_lines;
                break;
            case SDLK_P:
            case SDLK_SPACE:
                *pause = !*pause;
                break;
            default:
                break;
            }
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                *mouse_clicked = true;
                *x_mouse = (int)event.button.x;
                *y_mouse = (int)event.button.y;
            }
        }
    }
}

static void check_branded(struct sprite **sprites, int x_mouse, int y_mouse)
{
    for (int i = 0; i < NUM_IMAGES; i++) {
        if (!sprites[i]->sprite_base->can_be_branded || sprites[i]->is_branded)
            continue;

        /* Use actual sprite dimensions, not hardcoded values */
        int w = (int)sprites[i]->sprite_base->image_width;
        int h = (int)sprites[i]->sprite_base->image_height;
        double x_rhs = sprites[i]->x + w;
        double y_bot = sprites[i]->y + h;

        if (x_mouse >= (int)sprites[i]->x && x_mouse <= (int)x_rhs &&
            y_mouse >= (int)sprites[i]->y && y_mouse <= (int)y_bot) {
            sprites[i]->is_branded = true;
        }
    }
}

static void move(struct sprite *sprites[], int *pause, double elapsed_ticks)
{
    if (*pause) return;

    double elapsed_seconds = elapsed_ticks / NUM_MILLISECONDS;
    double distance = SPRITE_PIXELS_PER_SECOND * elapsed_seconds;

    for (int i = 0; i < NUM_IMAGES; i++) {
        xadd(sprites[i], distance);
    }
}

static Uint32 get_elapsed_ticks(void)
{
    static Uint32 prev_ticks = 0;
    static bool first_call = true;
    Uint32 cur_ticks = SDL_GetTicks();

    if (first_call) {
        /* First call: prime the clock, don't return total uptime */
        prev_ticks = cur_ticks;
        first_call = false;
        return 0;
    }

    Uint32 elapsed_ticks = cur_ticks - prev_ticks;
    prev_ticks = cur_ticks;
    return elapsed_ticks;
}

static void main_draw(SDL_Renderer *renderer, struct sprite **sprites,
                      SDL_Texture *brand)
{
    for (int i = 0; i < NUM_IMAGES; i++) {
        draw(renderer, sprites[i], brand);
    }
}

/* =========================================================================
 * setup_img / load_img — create sprite array from image directories
 * ========================================================================= */
static struct sprite **setup_img(SDL_Renderer *renderer)
{
    struct sprite **sprites = xcalloc(NUM_IMAGES, sizeof(struct sprite *));

    srand((unsigned int)time(nullptr));
    unsigned int x_scale = RAND_MAX / SCREEN_WIDTH;
    unsigned int y_scale = RAND_MAX / SCREEN_HEIGHT;

    for (int i = 0; i < NUM_IMAGES; i++) {
        sprites[i] = load_img(renderer, i, x_scale, y_scale);
    }
    return sprites;
}

static struct sprite *load_img(SDL_Renderer *renderer, int i,
                               unsigned int x_scale, unsigned int y_scale)
{
    SDL_Log("image_names[%d] = [%s]\n", i, image_names[i]);

    struct sprite_base *base = base_init(renderer, image_names[i]);
    if (!base) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't initialise base for: %s\n%s\n",
                     image_names[i], SDL_GetError());
        return nullptr;
    }

    struct sprite *sprite = sprite_init(base);
    if (!sprite) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't initialise sprite for: %s\n%s\n",
                     image_names[i], SDL_GetError());
        free_sprite_base(base);
        return nullptr;
    }

    unsigned int x = (unsigned int)rand();
    x /= x_scale;
    unsigned int y = (unsigned int)rand();
    y /= y_scale;

    /* Clouds stay in the sky (top half); animals on the ground (bottom half) */
    int half = SCREEN_HEIGHT / 2;
    if (i == IMG_CLOUD) {
        if (y > (unsigned int)half)
            y = (unsigned int)base->image_height;
    } else {
        if (y < (unsigned int)half)
            y = (unsigned int)(SCREEN_HEIGHT - base->image_height);
    }

    SDL_Log("\ty\t%d\n", (int)y);

    set(sprite, (double)x, (double)y);
    set_speed(sprite, 1.0f);
    return sprite;
}

/* =========================================================================
 * free_sprites — clean up entire sprite array
 * ========================================================================= */
static void free_sprites(struct sprite **sprites, int count)
{
    if (!sprites) return;
    for (int i = 0; i < count; i++) {
        free_sprite(sprites[i]);
    }
    free(sprites);
}
