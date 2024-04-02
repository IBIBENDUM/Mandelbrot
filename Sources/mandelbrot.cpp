#include <time.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <immintrin.h>

#include "utils.h"
#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "events_handlers.h"
#include "ui.h"
#include "palettes.h"
#include "calculations.h"

error_code init_graphic(Graphic* graphic)
{
    RETURN_IF_NULL(graphic, NULL_PTR_ERR);

    RETURN_IF_NULL(!SDL_Init(SDL_INIT_VIDEO), SDL_ERR);
    RETURN_IF_NULL(!TTF_Init(), SDL_ERR);

    SDL_Window*   window   = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    RETURN_IF_NULL(window && renderer, NULL_PTR_ERR);

    RETURN_IF_NULL(!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND), SDL_ERR);

    SDL_Surface* surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT,
                                                COLOR_DEPTH, 0, 0, 0, 0);
    RETURN_IF_NULL(surface, NULL_PTR_ERR);

    TTF_Font* font = TTF_OpenFont(WINDOW_FONT, WINDOW_FONT_SIZE);
    RETURN_IF_NULL(font, NULL_PTR_ERR);

    *graphic = (Graphic)
    {
        .window   = window,
        .surface  = surface,
        .renderer = renderer,
        .font     = font,
    };
    return NO_ERR;
}

error_code init_mandelbrot(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    Graphic graphic = {};
    RETURN_IF_NULL(!init_graphic(&graphic), SDL_ERR);

    Screen screen =
    {
        .graphic = graphic,
        .height  = graphic.surface->h,
        .width   = graphic.surface->w,
        .pos_x   = 0,
        .pos_y   = 0,
        .zoom    = DEFAULT_ZOOM
    };

    uint32_t* palettes = get_palettes();

    *mandelbrot = {
        .is_running  = true,
        .show_debug  = true,
        .cur_calc    = DEFAULT_CALC_FUNC,
        .calc_func   = CALC_FUNCS[DEFAULT_CALC_FUNC],
        .screen      = screen,
        .cur_palette = DEFAULT_PALETTE,
        .palettes    = palettes,
    };

    return NO_ERR;
}

error_code destruct_mandelbrot(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    free(mandelbrot->palettes);

    TTF_CloseFont(mandelbrot->screen.graphic.font);
    SDL_FreeSurface(mandelbrot->screen.graphic.surface);
    SDL_DestroyRenderer(mandelbrot->screen.graphic.renderer);
    SDL_DestroyWindow(mandelbrot->screen.graphic.window);
    SDL_Quit();

    return NO_ERR;
}

static error_code draw_mandelbrot(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;
    SDL_Surface*  surface  = mandelbrot->screen.graphic.surface;

    RETURN_IF_NULL(!SDL_LockSurface(surface), SDL_ERR);

    SDL_Texture* rgb_texture = SDL_CreateTextureFromSurface(renderer, surface);

    clock_t tic_start = clock();

    mandelbrot->calc_func(mandelbrot);

    clock_t tic_end = clock();
    mandelbrot->screen.ticks = tic_end - tic_start;

    SDL_RenderCopy(renderer, rgb_texture, NULL, NULL);
    SDL_DestroyTexture(rgb_texture);

    SDL_UnlockSurface(surface);

    return NO_ERR;
}

error_code process_mandelbrot(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;
    SDL_Surface*  surface  = mandelbrot->screen.graphic.surface;

    handle_events(mandelbrot);

    if (draw_mandelbrot(mandelbrot) != NO_ERR)
        mandelbrot->is_running = false;

    if (mandelbrot->show_debug)
        draw_debug_text(mandelbrot);

    if (mandelbrot->cur_palette == PALETTE_ANIMATED)
    {
        update_animated_palette(mandelbrot);
    }
    SDL_RenderPresent(renderer);

    return NO_ERR;
}
