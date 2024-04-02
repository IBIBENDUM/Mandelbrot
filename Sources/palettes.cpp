#include <time.h>
#include <math.h>

#include "palettes.h"
#include "mandelbrot.h"
static void generate_even_palette(uint32_t* palettes)
{
    for (int i = 0; i < COLORS_NUMBER; i++)
        *(palettes + PALETTE_EVEN * COLORS_NUMBER + i) = (i % 2) ? 0xFFFFFFFF : 0x00000000;
}

static void generate_linear_palette(uint32_t* palettes)
{
<<<<<<< HEAD
    color_t start = { .hex = 0xFFFFFF };
    color_t end   = { .hex = 0x000000 };
=======
    color_t start = START_COLOR;
    color_t end   = END_COLOR;
>>>>>>> 248d2ea (review: report many problems and suggest many improvements)

    for (int i = 0; i < COLORS_NUMBER; i++)
        *(palettes + PALETTE_LINEAR * COLORS_NUMBER + i) =
        (start.a + i * (end.a - start.a) / COLORS_NUMBER) << ALPHA |
        (start.r + i * (end.r - start.r) / COLORS_NUMBER) << RED   |
        (start.g + i * (end.g - start.g) / COLORS_NUMBER) << GREEN |
        (start.b + i * (end.b - start.b) / COLORS_NUMBER) << BLUE;
}


void update_animated_palette(Mandelbrot* mandelbrot)
{
    color_t start = START_COLOR;
    color_t end   = END_COLOR;

    static double offset = 0;
    offset += ANIMATION_SPEED * mandelbrot->screen.ticks / CLOCKS_PER_SEC;

    for (int i = 0; i < COLORS_NUMBER; i++)
    {
        int ratio = ((i + (int) offset) % 10);
        *(mandelbrot->palettes + PALETTE_ANIMATED * COLORS_NUMBER + i) =
        (char) (start.a * (1 - ratio) + (end.a * ratio)) << ALPHA |
        (char) (start.r * (1 - ratio) + (end.r * ratio)) << RED   |
        (char) (start.g * (1 - ratio) + (end.g * ratio)) << GREEN |
        (char) (start.b * (1 - ratio) + (end.b * ratio)) << BLUE;
    }
}

uint32_t* get_palettes()
{
    uint32_t* palettes = (uint32_t*) calloc(PALETTES_AMOUNT * COLORS_NUMBER, sizeof(palettes[0]));
    generate_even_palette(palettes);
    generate_linear_palette(palettes);

    return palettes;
}

uint32_t* get_cur_palette(uint32_t* palettes, Palette cur_palette)
{
    return (palettes + cur_palette * COLORS_NUMBER);
}
