#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#include "mandelbrot.h"
#include "calculations.h"
#include "benchmark.h"

const size_t DEBUG_BUF_SIZE = 100;

static error_code store_text_background(Mandelbrot* mandelbrot, SDL_Surface* surface, int x, int y)
{
    RETURN_IF_NULL(mandelbrot && surface, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;

    SDL_Rect bg_rect = {.x = x - TEXT_BG_OFFSET, .y = y - TEXT_BG_OFFSET,
                        .w = surface->w + 2 * TEXT_BG_OFFSET,
                        .h = surface->h + 2 * TEXT_BG_OFFSET};

    SDL_SetRenderDrawColor(renderer, TEXT_BG_OPACITY, TEXT_BG_OPACITY,
                                     TEXT_BG_OPACITY, TEXT_BG_OPACITY);

    SDL_RenderFillRect(renderer, &bg_rect);

    return NO_ERR;
}

error_code store_text_with_background(SDL_Renderer* renderer, SDL_Surface* surface,
                                      int x, int y)
{
    RETURN_IF_NULL(renderer && surface, NULL_PTR_ERR);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    RETURN_IF_NULL(texture, SDL_ERR);

    SDL_Rect text_rect = {x, y, surface->w, surface->h};
    RETURN_IF_NULL(!SDL_RenderCopy(renderer, texture, NULL, &text_rect), SDL_ERR);

    SDL_DestroyTexture(texture);

    return NO_ERR;
}

error_code draw_text_with_background(Mandelbrot* mandelbrot, const char* text,
                                     int x, int y, int width)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;
    TTF_Font*     font     = mandelbrot->screen.graphic.font;

    RETURN_IF_NULL(font,     NULL_PTR_ERR);
    RETURN_IF_NULL(renderer, NULL_PTR_ERR);

    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, WINDOW_FONT_COLOR, width);
    RETURN_IF_NULL(surface, NULL_PTR_ERR);

    RETURN_IF_NULL(!store_text_background(mandelbrot, surface, x, y), SDL_ERR);

    RETURN_IF_NULL(!store_text_with_background(renderer, surface, x, y), SDL_ERR);

    SDL_FreeSurface(surface);

    return NO_ERR;
}

error_code draw_benchmark_text(Mandelbrot* mandelbrot)
{
    int height = mandelbrot->screen.height;
    int width = mandelbrot->screen.width;
    const char text[] = "Benchmarking...";

    int x = (width - sizeof(text) * WINDOW_FONT_SIZE / 2) / 2;
    int y = (height - WINDOW_FONT_SIZE) / 2;
    draw_text_with_background(mandelbrot, text, x, y, TEXT_WIDTH);

    return NO_ERR;
}

error_code draw_benchmark_results(Mandelbrot* mandelbrot, int tics_amount)
{
    int height = mandelbrot->screen.height;
    int width  = mandelbrot->screen.width;
    char debug_text[DEBUG_BUF_SIZE] = {};
    sprintf(debug_text, "Ticks on %d calls: %d\n Ticks per call: %lf\n Press any key to continue...",
            FUNCTIONS_RUNS_NUMBER, tics_amount, tics_amount / (float) FUNCTIONS_RUNS_NUMBER);

    draw_text_with_background(mandelbrot, debug_text, TEXT_BG_OFFSET, TEXT_BG_OFFSET, TEXT_WIDTH);

    return NO_ERR;
}

error_code draw_debug_text(Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    SDL_Renderer* renderer = mandelbrot->screen.graphic.renderer;
    TTF_Font*     font = mandelbrot->screen.graphic.font;
    Mandelbrot_calculation_method id = mandelbrot->method.id;
    int x        = mandelbrot->camera.pos_x;
    int y        = mandelbrot->camera.pos_y;
    size_t ticks = mandelbrot->screen.ticks;

    char debug_text[DEBUG_BUF_SIZE] = {};
    sprintf(debug_text, "Coordinates: %d, %d\nMethod: %s\nTicks: %zu\nFPS: %ld",
                         x, y, METHODS_NAMES[id], ticks, CLOCKS_PER_SEC / ticks);

    draw_text_with_background(mandelbrot, debug_text, TEXT_BG_OFFSET, TEXT_BG_OFFSET, TEXT_WIDTH);

    return NO_ERR;
}
