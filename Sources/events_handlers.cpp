#include <SDL2/SDL.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "utils.h"

static error_code update_zoom(Mandelbrot* mandelbrot, int x, int y, float zoom)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    mandelbrot->screen->pos_x = x;
    mandelbrot->screen->pos_y = y;
    mandelbrot->screen->zoom  = zoom;
    mandelbrot->dx            = _mm256_mul_ps(_mm256_set1_ps(1 / zoom), DX_FACTOR);

    return NO_ERR;
}

error_code keyboard_handler(SDL_Event* event, Mandelbrot* mandelbrot)
{
    RET_IF_ERR(event && mandelbrot, NULL_PTR_ERR);

    switch(event->key.keysym.sym)
    {
        case SDLK_a: mandelbrot->screen->pos_x -= KBRD_COORD_STEP; break;
        case SDLK_d: mandelbrot->screen->pos_x += KBRD_COORD_STEP; break;
        case SDLK_w: mandelbrot->screen->pos_y -= KBRD_COORD_STEP; break;
        case SDLK_s: mandelbrot->screen->pos_y += KBRD_COORD_STEP; break;
        case SDLK_RETURN: {
            mandelbrot->cur_func = (Calc_algorithm) ((mandelbrot->cur_func + 1) % CALC_FUNCS_AMOUNT);
            mandelbrot->func     = CALC_FUNCS[mandelbrot->cur_func];
            break;
        }
        default: break;
    };

    return NO_ERR;
}

error_code scroll_handler(SDL_Event* event, Mandelbrot* mandelbrot)
{
    RET_IF_ERR(event && mandelbrot, NULL_PTR_ERR);

    Screen* screen = mandelbrot->screen;

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

    const float mouse_rel_x = screen->pos_x + mouse_x - center_x;
    const float mouse_rel_y = screen->pos_y + mouse_y - center_y;

    const float new_zoom = screen->zoom  * zoom_factor;
    const int   new_x    = screen->pos_x + mouse_rel_x * (zoom_factor - 1);
    const int   new_y    = screen->pos_y + mouse_rel_y * (zoom_factor - 1);
    update_zoom(mandelbrot, new_x, new_y, new_zoom);

    // screen->pos_x += mouse_rel_x * (zoom_factor - 1);
    // screen->pos_y += mouse_rel_y * (zoom_factor - 1);
    // screen->zoom  *= zoom_factor;

    return NO_ERR;
}

error_code movement_handler(SDL_Event* event, Screen* screen)
{
    RET_IF_ERR(event && screen, NULL_PTR_ERR);

    screen->pos_x -= event->motion.xrel;
    screen->pos_y -= event->motion.yrel;

    return NO_ERR;
}
