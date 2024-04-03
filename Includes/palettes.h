#ifndef PALETTES_H_
#define PALETTES_H_

#include <stdint.h>

const char ALPHA = 24;
const char RED   = 16;
const char GREEN =  8;
const char BLUE  =  0;

union color_t
{
    uint32_t hex;
    uint8_t  bytes[4];
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
};

enum Palette_id
{
    PALETTE_EVEN,
    PALETTE_LINEAR,
    PALETTE_ANIMATED,
};
const int PALETTES_AMOUNT = sizeof(Palette_id);

#include "mandelbrot_config.h"
const int COLORS_NUMBER = MAX_ITERATION_NUMBER + 1;  // Plus one colour for points outside the radius

struct Palette
{
    Palette_id id;
    uint32_t start[COLORS_NUMBER * sizeof(color_t)];
};

void update_animated_palette(Mandelbrot*);

uint32_t* get_cur_palette(const Palette*);

void get_palettes(Palette*);

#endif // PALETTES_H_
