#include <SDL2/SDL.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"

int keyboard_handler(SDL_Event* event, Mandelbrot* mandelbrot)
{
    // BAH: error handlers

    switch(event->key.keysym.sym)
    {
        case SDLK_a: mandelbrot->screen->x_pos -= KBRD_COORD_STEP; break;
        case SDLK_d: mandelbrot->screen->x_pos += KBRD_COORD_STEP; break;
        case SDLK_w: mandelbrot->screen->y_pos -= KBRD_COORD_STEP; break;
        case SDLK_s: mandelbrot->screen->y_pos += KBRD_COORD_STEP; break;
        default: break;
    };

    return 0; // BAH: error codes
}

int scroll_handler(SDL_Event* event, Screen* screen)
{
    // BAH: error handlers
    if(event->wheel.y > 0)
        screen->zoom += ZOOM_FACTOR;
    else
        screen->zoom -= ZOOM_FACTOR;

    // BAH: Add cursor based zoom
    // int mouse_x_pos = 0;
    // int mouse_y_pos = 0;
    // SDL_GetMouseState(&mouse_x_pos, &mouse_y_pos);

    return 0; // BAH: error codes
}
