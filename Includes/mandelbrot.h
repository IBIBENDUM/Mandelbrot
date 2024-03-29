#ifndef MANDELBROT_H_
#define MANDELBROT_H_

struct Screen
{
    int       height;
    int       width;

    int       x_pos;
    int       y_pos;

    int       x_speed;
    int       y_speed;

    double    zoom;

    uint32_t* vmem;
};

struct Mandelbrot
{
    bool      is_running;

    Screen*   screen;
    uint32_t* palette;
};

enum error_code
{
    NO_ERR,
    NULL_PTR_ERR,
    SDL_ERR,
};

error_code draw_mandelbrot(SDL_Surface* surface, Mandelbrot* mandelbrot);


#endif // MANDELBROT_H_

