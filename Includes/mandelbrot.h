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

struct Camera
{
    int    pos_x;
    int    pos_y;
    double zoom;
};

struct Screen
{
    Graphic   graphic;

    int       height;
    int       width;

    size_t    ticks;
};

struct Mandelbrot
{
    bool      is_running;
    bool      show_debug;

    struct Method
    {
        Mandelbrot_calculation_method   id;
        Mandelbrot_calculation_function calculate;
    } method;

    Screen  screen;

    Camera  camera;

    Palette palette;
};

error_code init_mandelbrot(Mandelbrot*);

error_code destruct_mandelbrot(Mandelbrot*);

error_code process_mandelbrot(Mandelbrot*);

#endif // MANDELBROT_H_

