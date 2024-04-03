#include <immintrin.h>

#include "calculations.h"
#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "utils.h"

const int PARALLEL_PIXELS_NUMBER = 8; // Number of pixels processed in parallel

float MAX_RADIUS_2_VECT[PARALLEL_PIXELS_NUMBER] = {};

const __m256 MAX_RADIUS_2_256 = _mm256_set1_ps(MAX_RADIUS2);
const __m256 DX_FACTOR        = _mm256_set_ps(7.0,  6.0,  5.0,  4.0, 3.0,  2.0,  1.0,  0.0);

void __attribute__((constructor)) INITIALIZE_MAX_RADIUS_2_VECT()
{
    for (size_t i = 0; i < PARALLEL_PIXELS_NUMBER; i++)
        MAX_RADIUS_2_VECT[i] = MAX_RADIUS2;
}

// = Primitive implementation ==================================================

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

        x = x2 - y2 + x0;
        y = xy + xy + y0;

        iteration++;
    }
    return iteration;
}

error_code calc_mandelbrot_primitive(const Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    uint32_t* palette     = get_cur_palette(&mandelbrot->palette);
    Screen    screen      = mandelbrot->screen;
    Camera    camera      = mandelbrot->camera;
    uint32_t* vmem_buffer = (uint32_t*) screen.graphic.surface->pixels;

    for (int iy = 0; iy < screen.height; iy++)
    {
        const float y0 = (iy - screen.height / 2.0f + camera.pos_y) / camera.zoom;
        for (int ix = 0; ix < screen.width; ix++)
        {
            const float x0 = (ix - screen.width / 2.0f + camera.pos_x) / camera.zoom;

            size_t iteration = calc_mandelbrot_point_primitive(x0, y0);

            *vmem_buffer = palette[iteration];
            vmem_buffer++;
        }
    }
    return NO_ERR;
}

//= AVX2 with overloaded operators implementation ==============================

union mmxi_t;

union mmxf_t
{
    __m256 m;
    mmxf_t (__m256 val);
    mmxf_t (float val);
    mmxf_t (double val);
    operator __m256() const;
    operator mmxi_t() const;
};

union mmxi_t
{
    __m256i m;
    mmxi_t (__m256i val);
    mmxi_t (int val);
    operator __m256i() const {return m; };
    mmxi_t() : m(_mm256_setzero_si256()) {}
};

inline mmxf_t::mmxf_t (__m256 val)  : m (val) {}
inline mmxf_t::mmxf_t (float  val)  : m (_mm256_set1_ps (val)) {}
inline mmxf_t::mmxf_t (double  val) : m (_mm256_set1_ps ((float)val)) {}

inline mmxf_t::operator __m256() const { return m; }
inline mmxf_t::operator mmxi_t() const { return mmxi_t (_mm256_castps_si256(m)); };

inline mmxi_t::mmxi_t (__m256i val) : m (val) {}

inline mmxf_t operator + (const mmxf_t& a, const mmxf_t& b) { return _mm256_add_ps(a, b);             }
inline mmxf_t operator - (const mmxf_t& a, const mmxf_t& b) { return _mm256_sub_ps(a, b);             }
inline mmxf_t operator * (const mmxf_t& a, const mmxf_t& b) { return _mm256_mul_ps(a, b);             }
inline mmxf_t operator / (const mmxf_t& a, const mmxf_t& b) { return _mm256_div_ps(a, b);             }
inline mmxf_t operator < (const mmxf_t& a, const mmxf_t& b) { return _mm256_cmp_ps(a, b, _CMP_LT_OQ); }
inline bool   operator ! (const mmxf_t& a)                  { return _mm256_testz_ps(a, a);           }

inline mmxi_t operator -= (mmxi_t& a, const mmxi_t& b) { a = _mm256_sub_epi32(a, b); return a; }
inline mmxi_t gather_array(const void* src, const mmxi_t& dest)
{
  return _mm256_i32gather_epi32((const int*) src, dest, sizeof(uint32_t));
}
inline void store_array(const mmxi_t& src, void* dest)
{
  _mm256_store_si256((__m256i*)dest, src);
}

static mmxi_t calc_mandelbrot_point_AVX2_overload_ops(const mmxf_t x0, const mmxf_t y0)
{
    mmxf_t x = x0;
    mmxf_t y = y0;

    mmxi_t iterations; // iterations == 0 by default;

    for (int i = 0; i < MAX_ITERATION_NUMBER; i++)
    {
        const mmxf_t x2 = x * x;
        const mmxf_t y2 = y * y;
        const mmxf_t xy = x * y;
        const mmxf_t radius2 = x2 + y2;

        const mmxf_t cmp_mask = radius2 < (mmxf_t) MAX_RADIUS_2_256;
        if (!cmp_mask)
            break;

        y = xy + xy + y0;
        x = x2 - y2 + x0;

        iterations -= cmp_mask; // Each cmp_mask element is equals 0 or F...Fh = -1d
    }
    return iterations;
}

error_code calc_mandelbrot_AVX2_with_overloaded_operators(const Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    uint32_t*    palette     = get_cur_palette(&mandelbrot->palette);
    Screen       screen      = mandelbrot->screen;
    Camera       camera      = mandelbrot->camera;
    mmxi_t*      vmem_buffer = (mmxi_t*) screen.graphic.surface->pixels;
    const float  coord_x     = camera.pos_x - screen.width  / 2.0f;
    const float  coord_y     = camera.pos_y - screen.height / 2.0f;
    const mmxf_t dx          = DX_FACTOR / (float) camera.zoom;     // BAH: why can't it convert double to mmxf_t?

    for (int iy = 0; iy < screen.height; iy++)
    {
        mmxf_t y0 = (iy + coord_y) / camera.zoom;

        for (int ix = 0; ix < screen.width; ix += PARALLEL_PIXELS_NUMBER)
        {
            mmxf_t x0 = ((ix + coord_x) / camera.zoom) + dx;
            mmxi_t iterations = calc_mandelbrot_point_AVX2_overload_ops(x0, y0);

            mmxi_t colors = gather_array(palette, iterations);
            store_array(colors, vmem_buffer);

            vmem_buffer++;
        }
    }
    return NO_ERR;
}

// = AVX2 implementation =======================================================

static __m256i calc_mandelbrot_point_AVX2(const __m256 x0, const __m256 y0)
{
    __m256 x = x0;
    __m256 y = y0;

    __m256i iterations = _mm256_setzero_si256();

    for (int iteration = 0; iteration < MAX_ITERATION_NUMBER; iteration++)
    {
        const __m256 x2      = _mm256_mul_ps(x,  x);
        const __m256 y2      = _mm256_mul_ps(y,  y);
        const __m256 xy      = _mm256_mul_ps(x,  y);
        const __m256 radius2 = _mm256_add_ps(x2, y2);

        __m256 cmp_mask = _mm256_cmp_ps(radius2, MAX_RADIUS_2_256, _CMP_LT_OQ);
        if (_mm256_testz_ps(cmp_mask, cmp_mask))
            break;

        x = _mm256_add_ps(x0, _mm256_sub_ps(x2, y2));
        y = _mm256_add_ps(y0, _mm256_add_ps(xy, xy));

        iterations = _mm256_sub_epi32(iterations, _mm256_castps_si256(cmp_mask));
    }
    return iterations;
}

error_code calc_mandelbrot_AVX2(const Mandelbrot* mandelbrot)
{
    RETURN_IF_NULL(mandelbrot, NULL_PTR_ERR);

    uint32_t*    palette     = get_cur_palette(&mandelbrot->palette);
    Screen       screen      = mandelbrot->screen;
    Camera       camera      = mandelbrot->camera;
    __m256i*     vmem_buffer = (__m256i*) screen.graphic.surface->pixels;
    const float  coord_x     = camera.pos_x - screen.width  / 2.0f;
    const float  coord_y     = camera.pos_y - screen.height / 2.0f;
    const __m256 dx          = _mm256_mul_ps(_mm256_set1_ps(1 / camera.zoom), DX_FACTOR);

    for (int iy = 0; iy < screen.height; iy++)
    {
        const __m256 y0 = _mm256_set1_ps((iy + coord_y) / camera.zoom);

        for (int ix = 0; ix < screen.width; ix += PARALLEL_PIXELS_NUMBER)
        {
            __m256 x0 = _mm256_set1_ps((ix + coord_x) / camera.zoom);
            x0 = _mm256_add_ps(x0, dx);

            __m256i iterations = calc_mandelbrot_point_AVX2(x0, y0);

            __m256i colors = _mm256_i32gather_epi32((const int*) palette, iterations, sizeof(uint32_t));
            _mm256_store_si256(vmem_buffer, colors);

            vmem_buffer++;
        }
    }
    return NO_ERR;
}

// = Vectorized without SIMD implementation ====================================

#define PARALLEL_INSTRUCTION_                           \
    for (size_t i = 0; i < PARALLEL_PIXELS_NUMBER; i++) \

#define INIT_VECTOR(NAME, ...)                          \
    float NAME[PARALLEL_PIXELS_NUMBER] = {};            \
    PARALLEL_INSTRUCTION_ NAME[i] = __VA_ARGS__;

error_code calc_mandelbrot_vectorized(const Mandelbrot* mandelbrot)
{
    uint32_t*   palette     = get_cur_palette(&mandelbrot->palette);
    Screen      screen      = mandelbrot->screen;
    Camera      camera      = mandelbrot->camera;
    uint32_t*   vmem_buffer = (uint32_t*) screen.graphic.surface->pixels;
    const float coord_x     = camera.pos_x - screen.width  / 2.0f;
    const float coord_y     = camera.pos_y - screen.height / 2.0f;

    for (int iy = 0; iy < screen.height; iy++)
    {
        INIT_VECTOR(y0, (iy + coord_y) / camera.zoom);

        for (int ix = 0; ix < screen.width; ix += PARALLEL_PIXELS_NUMBER)
        {
            INIT_VECTOR(x0, (ix + coord_x + i) / camera.zoom);
            INIT_VECTOR(x, x0[i]);
            INIT_VECTOR(y, y0[i]);

            int iterations[PARALLEL_PIXELS_NUMBER] = {};

            for (int iteration = 0; iteration < MAX_ITERATION_NUMBER; iteration++)
            {
                INIT_VECTOR(x2, x[i] * x[i]);
                INIT_VECTOR(y2, y[i] * y[i]);
                INIT_VECTOR(xy, x[i] * y[i]);
                INIT_VECTOR(radius2, x2[i] + y2[i]);

                int cmp[PARALLEL_PIXELS_NUMBER] = {};
                PARALLEL_INSTRUCTION_{cmp[i] = radius2[i] < MAX_RADIUS_2_VECT[i];}
                char mask = 0;
                PARALLEL_INSTRUCTION_{mask += cmp[i];}
                if (!mask) break;

                PARALLEL_INSTRUCTION_{x[i] = x0[i] + x2[i] - y2[i];}
                PARALLEL_INSTRUCTION_{y[i] = y0[i] + xy[i] + xy[i];}

                PARALLEL_INSTRUCTION_{iterations[i] += cmp[i];}
            }

            PARALLEL_INSTRUCTION_{*(vmem_buffer + i) = *(palette + iterations[i]);}
            vmem_buffer += PARALLEL_PIXELS_NUMBER;
        }
    }
    return NO_ERR;
}

#undef PARALLEL_INSTRUCTION_
