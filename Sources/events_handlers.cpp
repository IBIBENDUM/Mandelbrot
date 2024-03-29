#include <SDL2/SDL.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "utils.h"

error_code keyboard_handler(SDL_Event* event, Mandelbrot* mandelbrot)
{
    RET_IF_ERR(event && mandelbrot, NULL_PTR_ERR);

    switch(event->key.keysym.sym)
    {
        case SDLK_a: mandelbrot->screen->x_pos -= KBRD_COORD_STEP; break;
        case SDLK_d: mandelbrot->screen->x_pos += KBRD_COORD_STEP; break;
        case SDLK_w: mandelbrot->screen->y_pos -= KBRD_COORD_STEP; break;
        case SDLK_s: mandelbrot->screen->y_pos += KBRD_COORD_STEP; break;
        default: break;
    };

    return NO_ERR;
}

error_code scroll_handler(SDL_Event* event, Screen* screen)
{
    RET_IF_ERR(event && screen, NULL_PTR_ERR);

    float zoom_factor = 1;
    if(event->wheel.y > 0)
        zoom_factor -= ZOOM_STEP;
    else
        zoom_factor += ZOOM_STEP;

    int mouse_x = 0;
    int mouse_y = 0;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    const float center_x = screen->width  / 2;
    const float center_y = screen->height / 2;

    const float mouse_rel_x = screen->x_pos + mouse_x - center_x;
    const float mouse_rel_y = screen->y_pos + mouse_y - center_y;

    screen->x_pos += mouse_rel_x * (zoom_factor - 1);
    screen->y_pos += mouse_rel_y * (zoom_factor - 1);
    screen->zoom  *= zoom_factor;

    return NO_ERR;
}


error_code movement_handler(SDL_Event* event, Screen* screen)
{
    RET_IF_ERR(event && screen, NULL_PTR_ERR);

    screen->x_pos -= event->motion.xrel;
    screen->y_pos -= event->motion.yrel;

    return NO_ERR;
}
