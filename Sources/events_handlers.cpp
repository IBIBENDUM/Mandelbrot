#include <SDL2/SDL.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "palettes.h"
#include "utils.h"
#include "benchmark.h"
#include "ui.h"
#include "calculations.h"

static error_code update_zoom(Mandelbrot* mandelbrot, int x, int y, float zoom)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    mandelbrot->camera.pos_x = x;
    mandelbrot->camera.pos_y = y;
    mandelbrot->camera.zoom  = zoom;

    return NO_ERR;
}

static error_code benchmark_handle(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer =  mandelbrot->screen.graphic.renderer;
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

static error_code change_method_handle(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    mandelbrot->method.id  = (Mandelbrot_calculation_method) ((mandelbrot->method.id + 1) % CALCULATION_FUNCTIONS_AMOUNT);
    mandelbrot->method.calculate = CALCULATION_FUNCTIONS[mandelbrot->method.id];

    return NO_ERR;
}

static error_code keyboard_handler(SDL_Event* event, Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(event && mandelbrot, NULL_PTR_ERR);

    switch(event->key.keysym.sym)
    {
        case SDLK_b:      benchmark_handle(mandelbrot); break;
        case SDLK_w:      mandelbrot->camera.pos_y -= KBRD_COORD_STEP;         break;
        case SDLK_a:      mandelbrot->camera.pos_x -= KBRD_COORD_STEP;         break;
        case SDLK_s:      mandelbrot->camera.pos_y += KBRD_COORD_STEP;         break;
        case SDLK_d:      mandelbrot->camera.pos_x += KBRD_COORD_STEP;         break;
        case SDLK_l:      mandelbrot->show_debug    = !mandelbrot->show_debug; break;
        case SDLK_1:      mandelbrot->palette.id   = PALETTE_EVEN;            break;
        case SDLK_2:      mandelbrot->palette.id   = PALETTE_LINEAR;          break;
        case SDLK_3:      mandelbrot->palette.id   = PALETTE_ANIMATED;        break;
        case SDLK_ESCAPE: mandelbrot->is_running    = false;                   break;
        case SDLK_RETURN: change_method_handle(mandelbrot);                    break;
        default:                                                               break;
    };

    return NO_ERR;
}

static error_code scroll_handler(SDL_Event* event, Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(event && mandelbrot, NULL_PTR_ERR);

    Screen screen = mandelbrot->screen;
    Camera camera = mandelbrot->camera;

    float zoom_factor = 1;
    if(event->wheel.y > 0)
        zoom_factor -= ZOOM_STEP;
    else
        zoom_factor += ZOOM_STEP;

    int mouse_x = 0;
    int mouse_y = 0;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    const float center_x = screen.width  / 2;
    const float center_y = screen.height / 2;

    const float mouse_rel_x = camera.pos_x + mouse_x - center_x;
    const float mouse_rel_y = camera.pos_y + mouse_y - center_y;

    const float new_zoom = camera.zoom  * zoom_factor;
    const int   new_x    = camera.pos_x + mouse_rel_x * (zoom_factor - 1);
    const int   new_y    = camera.pos_y + mouse_rel_y * (zoom_factor - 1);
    update_zoom(mandelbrot, new_x, new_y, new_zoom);

    return NO_ERR;
}

static error_code movement_handler(SDL_Event* event, Camera* camera)
{
    RETURN_IF_NULL(event && camera, NULL_PTR_ERR);

    camera->pos_x -= event->motion.xrel;
    camera->pos_y -= event->motion.yrel;

    return NO_ERR;
}

static error_code resize_handler(Mandelbrot* mandelbrot)
{
    // TODO: Your resize doesn't work in the least...
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    int width = 0;
    int height = 0;
    SDL_Window* window = mandelbrot->screen.graphic.window;
    SDL_GetWindowSize(window, &width, &height);
    width &= WIDTH_ALIGN_8;

    mandelbrot->screen.height = height;
    mandelbrot->screen.width  = width;
    SDL_SetWindowSize(window, width, height);
    mandelbrot->screen.graphic.surface = SDL_GetWindowSurface(window);

    return NO_ERR;
}

error_code handle_events(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

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
            case SDL_MOUSEMOTION: {
                if (event.motion.state & SDL_BUTTON_LMASK)
                    movement_handler(&event, &mandelbrot->camera);
                break;
            }
            case SDL_KEYDOWN: {
                keyboard_handler(&event, mandelbrot);
                break;
            }
            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    resize_handler(mandelbrot);   // BAH: Fix resize bug TODO: Yeah, fix it or remove it
                break;
            }
            default: break;
        }
    }

    return NO_ERR;
}

