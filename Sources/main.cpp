#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "debug_text.h"
#include "palettes.h"
#include "events_handlers.h"
#include "utils.h"


// BAH: clean up the code
int main()
{
    RET_IF_ERR(!SDL_Init(SDL_INIT_VIDEO), SDL_ERR);
    RET_IF_ERR(!TTF_Init(), SDL_ERR);

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    RET_IF_ERR(window && renderer, NULL_PTR_ERR);
    RET_IF_ERR(!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND), SDL_ERR);

    SDL_Surface* rgb_surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_DEPTH, 0, 0, 0, 0);
    RET_IF_ERR(rgb_surface, NULL_PTR_ERR);

    TTF_Font* font = TTF_OpenFont(WINDOW_FONT, WINDOW_FONT_SIZE);
    RET_IF_ERR(font, NULL_PTR_ERR);

    Mandelbrot* mandelbrot = init_mandelbrot(window, rgb_surface, renderer, font);
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    while (mandelbrot->is_running)
    {
        handle_events(mandelbrot);

        clock_t tic_start = clock();
        if (draw_mandelbrot(mandelbrot) != NO_ERR) break;
        clock_t tic_end = clock();
        mandelbrot->ticks = tic_end - tic_start;

        SDL_Texture* rgb_texture  = SDL_CreateTextureFromSurface(renderer, rgb_surface);
        SDL_RenderCopy(renderer, rgb_texture, NULL, NULL);
        SDL_DestroyTexture(rgb_texture);

        if (mandelbrot->show_debug)
            draw_debug_text(mandelbrot);

        SDL_RenderPresent(renderer);
    }

    destruct_mandelbrot(mandelbrot);

    TTF_CloseFont(font);
    SDL_FreeSurface(rgb_surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
