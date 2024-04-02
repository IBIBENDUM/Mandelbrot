#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#include "mandelbrot.h"
#include "calculations.h"
#include "benchmark.h"

// TODO: Just messy

const size_t DEBUG_BUF_SIZE = 100;

error_code draw_text_with_bg(Mandelbrot* mandelbrot, const char* text, int x, int y, int width)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;
    TTF_Font* font = mandelbrot->screen.graphic.font;

    SDL_Surface* text_surface = TTF_RenderText_Blended_Wrapped(font, text,
                                                               WINDOW_FONT_COLOR,
                                                               width);
    RETURN_IF_NULL(text_surface, NULL_PTR_ERR);

    int text_w   = text_surface->w;
    int text_h   = text_surface->h;

    SDL_Rect bg_rect = {.x = x - TEXT_BG_OFFSET, .y = y - TEXT_BG_OFFSET,
                        .w = text_w + 2 * TEXT_BG_OFFSET,
                        .h = text_h + 2 * TEXT_BG_OFFSET};

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 128);
    SDL_RenderFillRect(renderer, &bg_rect);


    SDL_Rect text_rect = {x, y, text_w, text_h};
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);

    return NO_ERR;
}

error_code draw_benchmark_text(Mandelbrot* mandelbrot)
{
    int height = mandelbrot->screen.height;
    int width = mandelbrot->screen.width;
    const char text[] = "Benchmarking...";
    draw_text_with_bg(mandelbrot, text, (width - sizeof(text) * WINDOW_FONT_SIZE / 2) / 2,
                     (height - WINDOW_FONT_SIZE) / 2, 150);

    return NO_ERR;
}

error_code draw_benchmark_results(Mandelbrot* mandelbrot, int tics_amount)
{
    int height = mandelbrot->screen.height;
    int width  = mandelbrot->screen.width;
    char debug_text[DEBUG_BUF_SIZE] = {};
    sprintf(debug_text, "Ticks on %d calls: %d\n Ticks per call: %lf\n Press any key to continue...",
            FUNCTIONS_RUNS_NUMBER, tics_amount, tics_amount / (float) FUNCTIONS_RUNS_NUMBER);

    draw_text_with_bg(mandelbrot, debug_text, TEXT_BG_OFFSET, TEXT_BG_OFFSET, 200);

    return NO_ERR;
}

error_code draw_debug_text(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;
    TTF_Font* font = mandelbrot->screen.graphic.font;
    Calc_implement alg = mandelbrot->cur_calc;
    const int x = mandelbrot->screen.pos_x;
    const int y = mandelbrot->screen.pos_y;
    const size_t ticks = mandelbrot->screen.ticks;

    char debug_text[DEBUG_BUF_SIZE] = {};
    sprintf(debug_text, "Coordinates: %d, %d\nMethod: %s\nTicks: %zu\nFPS: %ld",
                         x, y, METHODS_NAMES[alg], ticks, CLOCKS_PER_SEC / ticks);

    draw_text_with_bg(mandelbrot, debug_text, TEXT_BG_OFFSET, TEXT_BG_OFFSET, 200);

    return NO_ERR;
}
