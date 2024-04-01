#ifndef MANDELBROT_CONFIG_H_
#define MANDELBROT_CONFIG_H_

#include <SDL2/SDL.h>

// #include "mandelbrot.h"

const int WIDTH_ALIGN_8 = ~0x7;  // Align width by 8 so that AVX instructions do not overflow the buffer

const char* const    WINDOW_TITLE         = "mandelbrot!";
const int            WINDOW_HEIGHT        = 900;
const int            WINDOW_WIDTH         = 1600 & WIDTH_ALIGN_8;
const int            COLOR_DEPTH          = 32;
const char* const    WINDOW_FONT          = "Fonts/Roboto-Bold.ttf";
const int            WINDOW_FONT_SIZE     = 14;
const SDL_Color      WINDOW_FONT_COLOR    = {255, 255, 255, 255};
const int            TEXT_BG_OFFSET       = 10;

const float          DEFAULT_ZOOM         = 300.0;
const float          ZOOM_STEP            = 0.1;
const int            KBRD_COORD_STEP      = 10;

const int            MAX_ITERATION_NUMBER = 64;
const float          MAX_RADIUS2          = 4;

#include "palettes.h"
const Palette        DEFAULT_PALETTE      = PALETTE_EVEN;
const color_t        START_COLOR          = { .hex = 0xFFFFFF };
const color_t        END_COLOR            = { .hex = 0x000000 };

#include "calculations.h"
const Calc_implement DEFAULT_CALC_FUNC    = CALC_PRIMITIVE;

#endif // MANDELBROT_CONFIG_H_
