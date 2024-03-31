#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"
#include "palettes.h"

enum error_code
{
    NO_ERR,
    NULL_PTR_ERR,
    SDL_ERR,
};

struct Mandelbrot;
typedef error_code (*calc_algorithm_func)(const Mandelbrot*);
enum Calc_algorithm
{
    CALC_PRIMITIVE,
    CALC_AVX2,
    CALC_AVX2_OVERLOAD,
};

struct Screen
{
    SDL_Window*  window;
    SDL_Surface* surface;
    SDL_Renderer* renderer;
    TTF_Font*    font;

    int       height;
    int       width;

    int       pos_x;
    int       pos_y;

    int       speed_x;
    int       speed_y;

    double    zoom;
};

struct Mandelbrot
{
    bool      is_running;
    bool      show_debug;
    bool      show_help;

    Calc_algorithm      cur_calc;
    calc_algorithm_func calc_func;
    size_t ticks;

    Screen*  screen;

    Palette  cur_palette;
    uint32_t* palettes;

    __m256    dx;
};

error_code calc_mandelbrot_primitive(const Mandelbrot*);

error_code calc_mandelbrot_AVX2(const Mandelbrot*);

error_code calc_mandelbrot_AVX2_overload_ops(const Mandelbrot*);

const calc_algorithm_func CALC_FUNCS[] =
{
    calc_mandelbrot_primitive,
    calc_mandelbrot_AVX2,
    calc_mandelbrot_AVX2_overload_ops,
};
const int CALC_FUNCS_AMOUNT = sizeof(CALC_FUNCS) / sizeof(calc_algorithm_func);
const char* const METHODS_NAMES[CALC_FUNCS_AMOUNT] = {"Primitive", "AVX2", "AVX2 with overload"};


error_code draw_mandelbrot(const Mandelbrot*);

Mandelbrot* init_mandelbrot(SDL_Window*, SDL_Surface*, SDL_Renderer*, TTF_Font*); // BAH: Add structure for this
error_code  destruct_mandelbrot(Mandelbrot*);

#endif // MANDELBROT_H_

