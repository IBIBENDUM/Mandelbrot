#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"
#include "palettes.h"
#include "error_codes.h"
#include "calculations.h"

struct Graphic
{
    SDL_Window*   window;
    SDL_Surface*  surface;
    SDL_Renderer* renderer;
    TTF_Font*     font;
};

struct Screen
{
    Graphic   graphic;

    int       height;
    int       width;

    int       pos_x;
    int       pos_y;

    double    zoom;

    size_t ticks;
};

struct Mandelbrot
{
    bool      is_running;
    bool      show_debug;
    bool      show_help;

    Calc_implement      cur_calc;
    calc_implement_func calc_func;

    Screen  screen;

    Palette   cur_palette;
    uint32_t* palettes;

    __m256    dx;
};

error_code init_mandelbrot(Mandelbrot*);

error_code destruct_mandelbrot(Mandelbrot*);

error_code process_mandelbrot(Mandelbrot*);

error_code draw_mandelbrot(const Mandelbrot*);


#endif // MANDELBROT_H_

