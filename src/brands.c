#include <SDL_image.h>

#include "datadir.h"
#include "surface.h"
#include "util.h"
#include "wheelhouse.h"

#include "brands.h"

static SDL_Surface *create_wheelhouse_image(void)
{
	return IMG_ReadXPMFromArray(wheelhouse);
}

struct brands *setup_brands(void)
{
    struct brands *brands = xmalloc(sizeof(struct brands));

    brands->brand_wheelhouse = create_wheelhouse_image();
    if (!brands->brand_wheelhouse) {
        printf("Unable to create brand_wheelhouse\n");
        exit(EXIT_FAILURE);
    }

    brands->brand_wheelhouse_transparent = load_image(IMG_DIR "wheelhouse_image_transparent.png");
    if (!brands->brand_wheelhouse_transparent) {
        printf("Unable to load brand_wheelhouse_transparent\n");
        exit(EXIT_FAILURE);
    }

    brands->brand_wheelhouse_inverted = load_image(IMG_DIR "wheelhouse_image_inverted.png");
    if (!brands->brand_wheelhouse_inverted) {
        printf("Unable to load brand_wheelhouse_inverted\n");
        exit(EXIT_FAILURE);
    }

    brands->brand_wheelhouse_black_with_white_lines = load_image(IMG_DIR "wheelhouse_image_black_with_white_lines.png");
    if (!brands->brand_wheelhouse_black_with_white_lines) {
        printf("Unable to brand_wheelhouse_black_with_white_lines\n");
        exit(EXIT_FAILURE);
    }

    return brands;
}
