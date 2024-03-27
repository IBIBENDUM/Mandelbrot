#include <SDL2/SDL.h>

#include "mandelbrot.h"

error_code calc_mandelbrot_primitive(Screen* screen)
{
    // BAH: add check for nulls

    const float x_shift = screen->width  / 2.0;
    const float y_shift = screen->height / 2.0;

    for (int iy = 0; iy < screen->height; iy++)
    {
        for (int ix = 0; ix < screen->width; ix++)
        {
            const float x0 = ((float)ix - x_shift + screen->x_pos) / screen->zoom;
            const float y0 = ((float)iy - y_shift + screen->y_pos) / screen->zoom;

            float x = x0;
            float y = y0;

            size_t iteration = 0;

            while (iteration < MAX_ITERATION_NUMBER)
            {
                const float x2 = x * x;
                const float y2 = y * y;
                const float radius2 = x2 + y2;

                if (radius2 > MAX_RADIUS2)
                    break;

                y = 2*x*y + y0  ;
                x = x2 - y2 + x0;

                iteration++;
            }

            *(screen->vmem + iy * screen->width + ix) = (iteration % 2 == 0)? 0xFFFFFF: 0x000000;
        }
    }

    return NO_ERR;
}

error_code draw_mandelbrot(SDL_Surface* surface, Screen* screen)
{
    // BAH: add check for nulls
    SDL_LockSurface(surface);
    // BAH: add error handle

    calc_mandelbrot_primitive(screen);

    SDL_UnlockSurface(surface);
    // BAH: add error handle

    return NO_ERR;
}
