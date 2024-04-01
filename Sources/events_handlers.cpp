#include <SDL2/SDL.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "palettes.h"
#include "utils.h"
#include "benchmark.h"
#include "debug_text.h"

static error_code update_zoom(Mandelbrot* mandelbrot, int x, int y, float zoom)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    mandelbrot->screen->pos_x = x;
    mandelbrot->screen->pos_y = y;
    mandelbrot->screen->zoom  = zoom;
    mandelbrot->dx            = _mm256_mul_ps(_mm256_set1_ps(1 / zoom), DX_FACTOR);

    return NO_ERR;
}

static error_code benchmark_handle(Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer =  mandelbrot->screen->renderer;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    draw_benchmark_text(mandelbrot);
    SDL_RenderPresent(renderer);
    int ticks_amount = benchmark_function(mandelbrot);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    draw_benchmark_results(mandelbrot, ticks_amount);
    SDL_RenderPresent(renderer);

    bool key_pressed = false;
    SDL_Event event = {};
    while (!key_pressed)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_KEYDOWN: {
                key_pressed = true;
                break;
            }
            default: break;
        }
    }

    return NO_ERR;
}

static error_code keyboard_handler(SDL_Event* event, Mandelbrot* mandelbrot)
{
    RET_IF_ERR(event && mandelbrot, NULL_PTR_ERR);

    switch(event->key.keysym.sym)
    {
        case SDLK_w: mandelbrot->screen->pos_y -= KBRD_COORD_STEP; break;
        case SDLK_a: mandelbrot->screen->pos_x -= KBRD_COORD_STEP; break;
        case SDLK_s: mandelbrot->screen->pos_y += KBRD_COORD_STEP; break;
        case SDLK_d: mandelbrot->screen->pos_x += KBRD_COORD_STEP; break;
        case SDLK_ESCAPE: mandelbrot->is_running = false; break;
        case SDLK_b: benchmark_handle(mandelbrot); break;
        case SDLK_l: mandelbrot->show_debug = !mandelbrot->show_debug; break;
        case SDLK_1: mandelbrot->cur_palette    = PALETTE_EVEN; break;
        case SDLK_2: mandelbrot->cur_palette    = PALETTE_LINEAR; break;
        case SDLK_3: mandelbrot->cur_palette    = PALETTE_LINEAR_ANIMATED; break;
        case SDLK_RETURN: {
            mandelbrot->cur_calc  = (Calc_algorithm) ((mandelbrot->cur_calc + 1) % CALC_FUNCS_AMOUNT);
            mandelbrot->calc_func = CALC_FUNCS[mandelbrot->cur_calc];
            break;
        }
        default: break;
    };

    return NO_ERR;
}

static error_code scroll_handler(SDL_Event* event, Mandelbrot* mandelbrot)
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

    return NO_ERR;
}

static error_code movement_handler(SDL_Event* event, Screen* screen)
{
    RET_IF_ERR(event && screen, NULL_PTR_ERR);

    screen->pos_x -= event->motion.xrel;
    screen->pos_y -= event->motion.yrel;

    return NO_ERR;
}

static error_code resize_handler(Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    int width = 0;
    int height = 0;
    SDL_Window* window = mandelbrot->screen->window;
    SDL_GetWindowSize(window, &width, &height);
    width &= WIDTH_ALIGN_8;

    mandelbrot->screen->height = height;
    mandelbrot->screen->width  = width;
    SDL_SetWindowSize(window, width, height);
    mandelbrot->screen->surface = SDL_GetWindowSurface(window);

    return NO_ERR;
}

error_code handle_events(Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    SDL_Event event = {};

    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT: {
                mandelbrot->is_running = false;
                break;
            }
            case SDL_MOUSEWHEEL: {
                scroll_handler(&event, mandelbrot);
                break;
            }
            case SDL_MOUSEMOTION: {         //BAH: REMAKE THIS
                if (event.motion.state & SDL_BUTTON_LMASK)
                    movement_handler(&event, mandelbrot->screen);
                break;
            }
            case SDL_KEYDOWN: {
                keyboard_handler(&event, mandelbrot);
                break;
            }
            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    resize_handler(mandelbrot);   // BAH: Fix resize bug
                break;
            }
            default: break;
        }
    }

    return NO_ERR;
}

