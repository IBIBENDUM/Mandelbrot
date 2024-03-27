#include <SDL2/SDL.h>
#include <immintrin.h>

#include "utils.h"
#include "mandelbrot.h"
#include "mandelbrot_config.h"

const int PARALLEL_PIXELS_NUMBER = 8; // Number of pixels processed in parallel

static size_t calc_mandelbrot_point(int x0, int y0, Screen* screen)
{
    float x = x0;
    float y = y0;

    size_t iteration = 0;

    while (iteration < MAX_ITERATION_NUMBER)
    {
        const float x2 = x * x;
        const float y2 = y * y;
        const float radius2 = x2 + y2;

        if (radius2 > MAX_RADIUS2)
            break;

        y = xy + xy + y0;
        x = x2 - y2 + x0;

        iteration++;
    }
    return iteration;
}

error_code calc_mandelbrot_primitive(Screen* screen)
{
    RET_IF_ERR(screen, NULL_PTR_ERR);

    const float x_shift = screen->width  / 2.0;
    const float y_shift = screen->height / 2.0;

    for (int iy = 0; iy < screen->height; iy++)
    {
        const float y0 = ((float)iy - y_shift + screen->y_pos) / screen->zoom;
        for (int ix = 0; ix < screen->width; ix++)
        {
            const float x0 = ((float)ix - x_shift + screen->x_pos) / screen->zoom;

            size_t iteration = calc_mandelbrot_point(x0, y0, screen);

            uint32_t* pixel_ptr = screen->vmem + iy * screen->width + ix;
            if (iteration < MAX_ITERATION_NUMBER)
            {
                *pixel_ptr = (iteration % 2 == 0)? 0xFFFFFF: 0x000000;
            }
            else
                *pixel_ptr = 0;
        }
    }

    return NO_ERR;
}

error_code calc_mandelbrot_AVX2(Screen* screen)
{
    RET_IF_ERR(screen, NULL_PTR_ERR);

    const float x_shift = screen->width  / 2.0;
    const float y_shift = screen->height / 2.0;

    const float invert_zoom = 1 / screen->zoom;
    const __m256 DX = _mm256_mul_ps(_mm256_set1_ps(invert_zoom), DX_FACTOR);

    for (int iy = 0; iy < screen->height; iy++)
    {
        uint32_t* pixelsRow = (uint32_t*) screen->vmem;
        const __m256 y0 = _mm256_set1_ps(((float)iy - y_shift + screen->y_pos) / screen->zoom);

        for (int ix = 0; ix < screen->width; ix += PARALLEL_PIXELS_NUMBER)
        {
            const __m256 x0 = _mm256_add_ps(_mm256_set1_ps(((float)ix - x_shift + screen->x_pos) / screen->zoom), DX);

            __m256 x = x0;
            __m256 y = y0;

            __m256i _iterations = _mm256_setzero_si256();

            for (int iteration = 0; iteration < 256; iteration++)
            {
                const __m256 x2 = _mm256_mul_ps(x, x);
                const __m256 y2 = _mm256_mul_ps(y, y);
                const __m256 xy = _mm256_mul_ps(x, y);
                const __m256 radius2 = _mm256_add_ps(x2, y2);

                __m256 cmp_mask = _mm256_cmp_ps(radius2, MAX_RADIUS_2_256, _CMP_NGE_UQ);
                int          mask = _mm256_movemask_ps(cmp_mask);

                if (!mask)
                    break;

                x = _mm256_add_ps(x0, _mm256_sub_ps(x2, y2));
                y = _mm256_add_ps(y0, _mm256_add_ps(xy, xy));

                _iterations = _mm256_sub_epi32(_iterations, _mm256_castps_si256(cmp_mask));

            }

            int* iterations_arr = (int*)&_iterations;
            for (int i = 0; i < 8; i++)
            {
                uint32_t* pixel_ptr = screen->vmem + iy * screen->width + ix + i;
                if (iterations_arr[i] < MAX_ITERATION_NUMBER)
                {
                    *pixel_ptr = (iterations_arr[i] % 2 == 0)? 0xFFFFFF: 0x000000;
                }
                else
                    *pixel_ptr = 0;
            }
        }
        pixelsRow += PARALLEL_PIXELS_NUMBER;
    }

    return NO_ERR;
}

error_code draw_mandelbrot(SDL_Surface* surface, Screen* screen)
{
    RET_IF_ERR(surface && screen, NULL_PTR_ERR);

    RET_IF_ERR(!SDL_LockSurface(surface), SDL_ERR);

    // calc_mandelbrot_primitive(screen);
    calc_mandelbrot_AVX2(screen);

    SDL_UnlockSurface(surface);

    return NO_ERR;
}
