#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "palettes.h"
#include "events_handlers.h"
#include "utils.h"

SDL_Window* sdl_init_window();
SDL_Surface* sdl_get_surface(SDL_Window*);
void sdl_quit(SDL_Window* );

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = sdl_init_window();
    RET_IF_ERR(window, NULL_PTR_ERR);
// BAH: mb SDL_CreateWindowAndRenderer

    SDL_Surface* surface = sdl_get_surface(window);
    RET_IF_ERR(surface, NULL_PTR_ERR);

    Mandelbrot* mandelbrot = init_mandelbrot(window, surface);
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    while (mandelbrot->is_running)
    {
        handle_events(mandelbrot);

        if (draw_mandelbrot(mandelbrot)) break;

        SDL_UpdateWindowSurface(window);
    }

    destruct_mandelbrot(mandelbrot);

    sdl_quit(window);

    return 0;
}

SDL_Window* sdl_init_window()
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

SDL_Surface* sdl_get_surface(SDL_Window* window)
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

void sdl_quit(SDL_Window* window)
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}
