#include <malloc.h>
#include "palettes.h"

static void generate_even_palette(uint32_t* palettes)
{
    for (int i = 0; i < COLORS_NUMBER; i++)
        *(palettes + PALETTE_EVEN * COLORS_NUMBER + i) = (i % 2) ? START_COLOR.hex : END_COLOR.hex;
}

static void generate_linear_palette(uint32_t* palettes)
{
    color_t start = { .hex = START_COLOR };
    color_t end   = { .hex = END_COLOR };

    for (int i = 0; i < COLORS_NUMBER; i++)
        *(palettes + PALETTE_LINEAR * COLORS_NUMBER + i) =
        (start.a + i * (end.a - start.a) / COLORS_NUMBER) << ALPHA |
        (start.r + i * (end.r - start.r) / COLORS_NUMBER) << RED   |
        (start.g + i * (end.g - start.g) / COLORS_NUMBER) << GREEN |
        (start.b + i * (end.b - start.b) / COLORS_NUMBER) << BLUE;
}

#include <math.h>

void update_animated_palette(uint32_t* palettes)
{
    color_t start = { .hex = START_COLOR };
    color_t end   = { .hex = END_COLOR };

    static double offset = 0;
    offset += 0.03;
    for (int i = 0; i < COLORS_NUMBER; i++)
    {
        float ratio = (i + (sin(offset)) * COLORS_NUMBER) / (COLORS_NUMBER - 1);
        *(palettes + PALETTE_ANIMATED * COLORS_NUMBER + i) =
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
