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
    RET_IF_ERR(!SDL_Init(SDL_INIT_VIDEO), SDL_ERR);
    RET_IF_ERR(!TTF_Init(), SDL_ERR);

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    RET_IF_ERR(window && renderer, NULL_PTR_ERR);
    RET_IF_ERR(!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND), SDL_ERR);

    SDL_Surface* surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_DEPTH, 0, 0, 0, 0);
    RET_IF_ERR(surface, NULL_PTR_ERR);

    TTF_Font* font = TTF_OpenFont(WINDOW_FONT, WINDOW_FONT_SIZE);
    RET_IF_ERR(font, NULL_PTR_ERR);

    *graphic = (Graphic)
    {
        .window  = window,
        .surface = surface,
        .renderer = renderer,
        .font    = font,
    };
    return NO_ERR;
}

error_code init_mandelbrot(Mandelbrot* mandelbrot)
{
    Graphic graphic = {};
    RET_IF_ERR(!init_graphic(&graphic), SDL_ERR);

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
        .dx          = _mm256_mul_ps(_mm256_set1_ps(1 / DEFAULT_ZOOM), DX_FACTOR)
    };

    return NO_ERR;
}

error_code destruct_mandelbrot(Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    free(mandelbrot->palettes);

    TTF_CloseFont(mandelbrot->screen.graphic.font);
    SDL_FreeSurface(mandelbrot->screen.graphic.surface);
    SDL_DestroyRenderer(mandelbrot->screen.graphic.renderer);
    SDL_DestroyWindow(mandelbrot->screen.graphic.window);
    SDL_Quit();

    return NO_ERR;
}

error_code process_mandelbrot(Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;
    SDL_Surface*  surface  = mandelbrot->screen.graphic.surface;

    handle_events(mandelbrot);

    clock_t tic_start = clock();
    if (draw_mandelbrot(mandelbrot) != NO_ERR) mandelbrot->is_running == false;
    clock_t tic_end = clock();
    mandelbrot->screen.ticks = tic_end - tic_start;

    SDL_Texture* rgb_texture  = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, rgb_texture, NULL, NULL);
    SDL_DestroyTexture(rgb_texture);

    if (mandelbrot->show_debug)
        draw_debug_text(mandelbrot);

    if (mandelbrot->cur_palette == PALETTE_ANIMATED)
        update_animated_palette(mandelbrot->palettes);

    SDL_RenderPresent(renderer);

    return NO_ERR;
}

error_code draw_mandelbrot(const Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    RET_IF_ERR(!SDL_LockSurface(mandelbrot->screen.graphic.surface), SDL_ERR);

    mandelbrot->calc_func(mandelbrot);

    SDL_UnlockSurface(mandelbrot->screen.graphic.surface);

    return NO_ERR;
}
