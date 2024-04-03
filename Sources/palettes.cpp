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
    color_t start = START_COLOR;
    color_t end   = END_COLOR;

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
        *(mandelbrot->palette.start + PALETTE_ANIMATED * COLORS_NUMBER + i) =
        (char) (start.a * (1 - ratio) + (end.a * ratio)) << ALPHA |
        (char) (start.r * (1 - ratio) + (end.r * ratio)) << RED   |
        (char) (start.g * (1 - ratio) + (end.g * ratio)) << GREEN |
        (char) (start.b * (1 - ratio) + (end.b * ratio)) << BLUE;
    }
}

void get_palettes(Palette* palettes)
{
    generate_even_palette(palettes->start);
    generate_linear_palette(palettes->start);
}

uint32_t* get_cur_palette(const Palette* palette)
{
    return ((uint32_t*)palette->start + palette->id * COLORS_NUMBER);
}
