#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "palettes.h"
#include "events_handlers.h"
#include "utils.h"

// =============================================================================
SDL_Window* init_window();
SDL_Surface* get_surface(SDL_Window*);
// =============================================================================

int main()
{
    SDL_Window* window = init_window();
    RET_IF_ERR(window, NULL_PTR_ERR);

    SDL_Surface* surface = get_surface(window);
    RET_IF_ERR(surface, NULL_PTR_ERR);

    uint32_t* palettes = get_palettes();

    Screen screen =
    {
        .height = surface->h,
        .width  = surface->w,
        .pos_x  = 0,
        .pos_y  = 0,
        .zoom   = DEFAULT_ZOOM,
        .vmem   = (uint32_t*) surface->pixels
    };

    Mandelbrot mandelbrot =
    {
        .is_running  = true,
        .cur_calc    = CALC_AVX2,
        .calc_func   = CALC_FUNCS[CALC_AVX2],
        .screen      = &screen,
        .shift_x     = screen.width  / 2.0f,
        .shift_y     = screen.height / 2.0f,
        .cur_palette = DEFAULT_PALETTE,
        .palettes    = palettes,
        .dx          = _mm256_mul_ps(_mm256_set1_ps(1 / DEFAULT_ZOOM), DX_FACTOR)
    };

    SDL_Event event = {};

    Uint32 old_time = 0;
    while (mandelbrot.is_running)
    {
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT: {
                    mandelbrot.is_running = false;
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    scroll_handler(&event, &mandelbrot);
                    break;
                }
                case SDL_MOUSEMOTION: {         //BAH: REMAKE THIS
                    if (event.motion.state & SDL_BUTTON_LMASK)
                        movement_handler(&event, &screen);
                    break;
                }
                case SDL_KEYDOWN: {
                    keyboard_handler(&event, &mandelbrot);
                    break;
                }
                default: break;
            }
        }

        // clock_t tic_start = clock();
        if (draw_mandelbrot(surface, &mandelbrot)) break;
        // clock_t tic_end = clock();
        // fprintf(stderr, "Ellapsed %ld ticks\n", tic_end - tic_start);

        SDL_UpdateWindowSurface(window);
    }

    // free(mandelbrot.palettes);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

SDL_Window* init_window()
{
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                          WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE );
    if(!window)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }
    return window;
}

SDL_Surface* get_surface(SDL_Window* window)
{
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if(!surface)
    {
        fprintf(stderr, "Couldn't get the SDL surface associated with the window! "
                        "SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }
    return surface;
}
