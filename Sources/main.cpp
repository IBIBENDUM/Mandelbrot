#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "mandelbrot.h"
#include "mandelbrot_config.h"
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

        SDL_Surface* text_surface = TTF_RenderText_Blended_Wrapped(font, "Hello\nWorld!", WINDOW_FONT_COLOR, TEXT_BG_WIDTH);
        RET_IF_ERR(text_surface, NULL_PTR_ERR);

    SDL_Rect text_background = { .w = TEXT_BG_WIDTH, .h = TEXT_BG_HEIGHT };

    Mandelbrot* mandelbrot = init_mandelbrot(window, rgb_surface);
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    while (mandelbrot->is_running)
    {
        handle_events(mandelbrot);

        if (draw_mandelbrot(mandelbrot) != NO_ERR) break;

        SDL_Texture* rgb_texture  = SDL_CreateTextureFromSurface(renderer, rgb_surface);
        SDL_RenderCopy(renderer, rgb_texture, NULL, NULL);
        SDL_DestroyTexture(rgb_texture);

        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
        SDL_RenderFillRect(renderer, &text_background);

        SDL_Rect textRect = {20, 10, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &textRect);
        SDL_DestroyTexture(text_texture);

        SDL_RenderPresent(renderer);

    }

    destruct_mandelbrot(mandelbrot);

    TTF_CloseFont(font);
    SDL_FreeSurface(rgb_surface);
    SDL_FreeSurface(text_surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
