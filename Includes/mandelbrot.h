#ifndef MANDELBROT_H_
#define MANDELBROT_H_

struct Screen
{
    int height;
    int width;

    double    x_pos;
    double    y_pos;

    double    zoom;

    uint32_t* vmem;
};

struct Mandelbrot
{
    bool is_running;
    Screen* screen;
};

enum error_code
{
    NO_ERR,
    NULL_PTR_ERR,
    SDL_ERR,
};

error_code draw_mandelbrot(SDL_Surface* surface, Screen* screen);


#endif // MANDELBROT_H_

