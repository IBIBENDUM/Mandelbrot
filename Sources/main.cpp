#include <SDL2/SDL.h>
#include <stdio.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "events_handlers.h"

int main(int argc, char* args[])
{

    if(SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                          WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE );
    if(!window)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if(!surface)
    {
        printf("Couldn't get the SDL surface associated with the window! "
                "SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

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
        .screen = &screen
    };

    SDL_Event event = {};

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

        if (draw_mandelbrot(surface, &screen)) break;
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
