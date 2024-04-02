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

    // -------------
    int       pos_x; // TODO: OF SCREEN??
    int       pos_y;

    double    zoom;
    // ------------- TODO: this are two different entities

    size_t ticks; // TODO: frames?
};

struct Mandelbrot
{
    bool      is_running;
    bool      show_debug;
    bool      show_help;

    // TODO: separate in a struct
    Calc_implement      cur_calc; // TODO: cur? cur cur /current_calculation_method/ ? (naming)
    calc_implement_func calc_func; // TODO: /calculate/ mandelbrot->calculate(&mandelbrot);
                                   //       or even better mandelbrot->method.calculate(&mandelbrot) and
                                   //                      mandelbrot->method.id

    Screen  screen;

    Palette   cur_palette; // TODO: cur_
    uint32_t* palettes;

    __m256    dx; // TODO: What is this, as far as I know not all your backends use avx instructions so no reason for it here
};

error_code init_mandelbrot(Mandelbrot*);

error_code destruct_mandelbrot(Mandelbrot*);

error_code process_mandelbrot(Mandelbrot*);

error_code draw_mandelbrot(const Mandelbrot*);


#endif // MANDELBROT_H_

