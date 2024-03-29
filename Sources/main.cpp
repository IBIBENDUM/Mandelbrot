#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
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

    // BAH: make pallete create function
    uint32_t PALETTE[256] = {};
    for (int i = 0; i < 256; i++)
        PALETTE[i] = (i % 2 == 0)? 0xFFFFFF: 0x000000;

    Screen screen =
    {
        .height = surface->h,
        .width  = surface->w,
        .x_pos  = 0,
        .y_pos  = 0,
        .zoom   = DEFAULT_ZOOM,
        .vmem   = (uint32_t*) surface->pixels
    };

    Mandelbrot mandelbrot =
    {
        .is_running = true,
        .screen     = &screen,
        .palette    = PALETTE
    };

    SDL_Event event = {};

    Uint32 old_time = 0;
    while (mandelbrot.is_running)
    {
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:       mandelbrot.is_running = false;         break;
                case SDL_MOUSEWHEEL: scroll_handler(&event, &screen);       break;
                case SDL_KEYDOWN:    keyboard_handler(&event, &mandelbrot); break;
                default: break;
            }
        }

        clock_t tic_start = clock();
        if (draw_mandelbrot(surface, &mandelbrot)) break;
        clock_t tic_end = clock();
        // fprintf(stderr, "Ellapsed %ld ticks\n", tic_end - tic_start);

        SDL_UpdateWindowSurface(window);
    }

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
