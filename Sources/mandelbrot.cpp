#include <SDL2/SDL.h>
#include <immintrin.h>

#include "utils.h"
#include "mandelbrot.h"
#include "mandelbrot_config.h"

const int PARALLEL_PIXELS_NUMBER = 8; // Number of pixels processed in parallel

static size_t calc_mandelbrot_point_primitive(const float x0, const float y0)
{
    float x = x0;
    float y = y0;

    size_t iteration = 0;

    while (iteration < MAX_ITERATION_NUMBER)
    {
        const float      x2 = x  * x;
        const float      y2 = y  * y;
        const float      xy = x  * y;
        const float radius2 = x2 + y2;

        if (radius2 > MAX_RADIUS2)
            break;

        y = xy + xy + y0;
        x = x2 - y2 + x0;

        iteration++;
    }
    return iteration;
}

error_code calc_mandelbrot_primitive(Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    uint32_t* palette = mandelbrot->palette;
    Screen*    screen = mandelbrot->screen;

    const float x_shift = screen->width  / 2.0;
    const float y_shift = screen->height / 2.0;

    for (int iy = 0; iy < screen->height; iy++)
    {
        const float y0 = ((float)iy - y_shift + screen->y_pos) / screen->zoom;
        for (int ix = 0; ix < screen->width; ix++)
        {
            const float x0 = ((float)ix - x_shift + screen->x_pos) / screen->zoom;

            size_t iteration = calc_mandelbrot_point_primitive(x0, y0);

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

// =============================================================================
union mmxi_t;

union mmxf_t
{
    __m256 m;
    mmxf_t (__m256 val);
    mmxf_t (float val);
    operator __m256() const {return m; };
    operator mmxi_t() const;
};

union mmxi_t
{
    __m256i m;
    mmxi_t (__m256i val);
    mmxi_t (int val);
    operator __m256i() const {return m; };
};

inline mmxf_t::mmxf_t (__m256 val)  : m (val) {}
inline mmxf_t::mmxf_t (float  val)  : m (_mm256_set1_ps (val)) {}

inline mmxi_t::mmxi_t (__m256i val) : m (val) {}
inline mmxi_t::mmxi_t (int     val) : m (_mm256_set1_epi32 (val)) {}

inline mmxf_t operator + (const mmxf_t a, const mmxf_t b) { return _mm256_add_ps(a, b); }
inline mmxf_t operator - (const mmxf_t a, const mmxf_t b) { return _mm256_sub_ps(a, b); }
inline mmxf_t operator * (const mmxf_t a, const mmxf_t b) { return _mm256_mul_ps(a, b); }
inline mmxf_t operator < (const mmxf_t a, const mmxf_t b) { return _mm256_cmp_ps(a, b, _CMP_NGE_UQ); }
// =============================================================================

static mmxi_t calc_mandelbrot_point_AVX2_overload_ops(const mmxf_t x0, const mmxf_t y0)
{
    mmxf_t x = x0;
    mmxf_t y = y0;

    mmxi_t iterations  = 0;

    for (int i = 0; i < MAX_ITERATION_NUMBER; i++)
    {
        const mmxf_t x2 = x * x;
        const mmxf_t y2 = y * y;
        const mmxf_t xy = x * y;
        const mmxf_t radius2 = x2 + y2;

        const mmxf_t cmp_mask = radius2 < MAX_RADIUS_2_256;
        int mask = _mm256_movemask_ps(cmp_mask);

        if (!mask)
            break;

        y = xy + xy + y0;
        x = x2 - y2 + x0;

        iterations = _mm256_sub_epi32(iterations, _mm256_castps_si256(cmp_mask));
    }
    return iterations;
}

static mmxi_t calc_mandelbrot_point_AVX2(const __m256 x0, const __m256 y0)
{
    __m256 x = x0;
    __m256 y = y0;

    __m256i iterations = _mm256_setzero_si256();

    for (int iteration = 0; iteration < 256; iteration++)
    {
        const __m256 x2      = _mm256_mul_ps(x,  x);
        const __m256 y2      = _mm256_mul_ps(y,  y);
        const __m256 xy      = _mm256_mul_ps(x,  y);
        const __m256 radius2 = _mm256_add_ps(x2, y2);

        __m256 cmp_mask = _mm256_cmp_ps(radius2, MAX_RADIUS_2_256, _CMP_NGE_UQ);
        int        mask = _mm256_movemask_ps(cmp_mask);

        if (!mask)
            break;

        x = _mm256_add_ps(x0, _mm256_sub_ps(x2, y2));
        y = _mm256_add_ps(y0, _mm256_add_ps(xy, xy));

        iterations = _mm256_sub_epi32(iterations, _mm256_castps_si256(cmp_mask));
    }

    return iterations;
}
error_code calc_mandelbrot_AVX2(Mandelbrot* mandelbrot)
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    uint32_t* palette = mandelbrot->palette;
    Screen*    screen = mandelbrot->screen;

    // BAH: remake this
    const float x_shift = screen->width  / 2;
    const float y_shift = screen->height / 2;

    const float invert_zoom = 1 / screen->zoom;
    const __m256 DX = _mm256_mul_ps(_mm256_set1_ps(invert_zoom), DX_FACTOR);

    __m256i* vmem_buffer = (__m256i*) screen->vmem;

    for (int iy = 0; iy < screen->height; iy++)
    {
        const __m256 y0 = _mm256_set1_ps(((float)iy - y_shift + screen->y_pos) / screen->zoom);

        for (int ix = 0; ix < screen->width; ix += PARALLEL_PIXELS_NUMBER)
        {
            const __m256 x0 = _mm256_add_ps(_mm256_set1_ps(((float)ix - x_shift + screen->x_pos) / screen->zoom), DX);

            __m256i iterations = calc_mandelbrot_point_AVX2(x0, y0);

            __m256i colors = _mm256_i32gather_epi32(palette, iterations, 4);
            _mm256_store_si256(vmem_buffer, colors);

            vmem_buffer++;
        }
    }

    return NO_ERR;
}

error_code draw_mandelbrot(SDL_Surface* surface, Mandelbrot* mandelbrot)
{
    RET_IF_ERR(surface && mandelbrot, NULL_PTR_ERR);

    RET_IF_ERR(!SDL_LockSurface(surface), SDL_ERR);

    // calc_mandelbrot_primitive(mandelbrot);

    calc_mandelbrot_AVX2(mandelbrot);

    SDL_UnlockSurface(surface);

    return NO_ERR;
}
