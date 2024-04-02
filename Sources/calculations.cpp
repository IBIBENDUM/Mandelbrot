#include "calculations.h"
#include "mandelbrot.h"
#include "mandelbrot_config.h"
#include "utils.h"

const int PARALLEL_PIXELS_NUMBER = 8; // Number of pixels processed in parallel

// BAH: HELP ME!
const float MAX_RADIUS_2_VECT[PARALLEL_PIXELS_NUMBER] =
{

    // #define _1 MAX_RADIUS2 TODO: this ain't no better, but much funnier and "efficient"
    // #define _2 _1, _1
    // #define _3 _2, _2
    // #define _4 _3, _3
    // _4
    MAX_RADIUS2, MAX_RADIUS2, MAX_RADIUS2, MAX_RADIUS2,
    MAX_RADIUS2, MAX_RADIUS2, MAX_RADIUS2, MAX_RADIUS2
};

// TODO: funny way to do it:
// void foo() __attribute__((constructor)) { memset(MAX_RADIUS_2_VECT, MAX_RADIUS2, sizeof(MAX_RADIUS_2_VECT) / sizeof(*MAX_RADIUS_2_VECT)); }

// TODO: all you could all in on newer C++ features
// template <typename type, size_t size>
// struct array_wrapper {
//     type array[size];
// };
// 
// template <typename type, size_t size>
// consteval array_wrapper<type, size> fill_array(auto &&value) {
//     array_wrapper<type, size> array;
//     for (auto& element: array.array)
//         element = std::forward<decltype(value)>(value);
//     return array;
// }
// 
// constexpr const auto my_array = fill_array<float, PARALLEL_PIXELS_NUMER>(MAX_RADIUS2);


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
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    uint32_t* palette = get_cur_palette(mandelbrot->palettes, mandelbrot->cur_palette);
    Screen    screen = mandelbrot->screen;
    uint32_t* vmem_buffer = (uint32_t*) screen.graphic.surface->pixels;

    for (int iy = 0; iy < screen.height; iy++)
    {
        const float y0 = ((float)iy - screen.height / 2.0f + screen.pos_y) / screen.zoom;
        for (int ix = 0; ix < screen.width; ix++)
        {
            const float x0 = ((float)ix - screen.width / 2.0f + screen.pos_x) / screen.zoom;

            size_t iteration = calc_mandelbrot_point_primitive(x0, y0);

            *vmem_buffer = palette[iteration];
            vmem_buffer++;
        }
    }

    return NO_ERR;
}

// = AVX2 with overloaded operators implementation =============================

union mmxi_t;

union mmxf_t
{
    __m256 m;
    mmxf_t (__m256 val);
    mmxf_t (float val);
    operator __m256() const {return m; };
    operator mmxi_t() const;
};

union mmxi_t // TODO: learn how to make operator overloadings faster (use cppinsights, look at clang ast, optview2, godbolt)
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
inline mmxf_t operator < (const mmxf_t a, const mmxf_t b) { return _mm256_cmp_ps(a, b, _CMP_LT_OQ); }

static mmxi_t calc_mandelbrot_point_AVX2_overload_ops(const mmxf_t x0, const mmxf_t y0)
{
    mmxf_t x = x0;
    mmxf_t y = y0;

    mmxi_t iterations  = _mm256_setzero_si256();

    for (int i = 0; i < MAX_ITERATION_NUMBER; i++)
    {
        const mmxf_t x2 = x * x;
        const mmxf_t y2 = y * y;
        const mmxf_t xy = x * y;
        const mmxf_t radius2 = x2 + y2;

        const mmxf_t cmp_mask = radius2 < MAX_RADIUS_2_256;
        if (_mm256_testz_ps(cmp_mask, cmp_mask))
            break;

        y = xy + xy + y0;
        x = x2 - y2 + x0;

        iterations = _mm256_sub_epi32(iterations, _mm256_castps_si256(cmp_mask));
    }
    return iterations;
}

error_code calc_mandelbrot_AVX2_overload_ops(const Mandelbrot* mandelbrot) // TODO: fix it
{
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    Screen     screen  = mandelbrot->screen;
    uint32_t* palette =  get_cur_palette(mandelbrot->palettes, mandelbrot->cur_palette);
    const float coord_x = screen.pos_x - screen.width / 2.0f;
    const float coord_y = screen.pos_y - screen.height / 2.0f;

    mmxi_t* vmem_buffer = (mmxi_t*) screen.graphic.surface->pixels;

    for (int iy = 0; iy < screen.height; iy++)
    {
        mmxf_t y0 = (iy + coord_y) / screen.zoom;

        for (int ix = 0; ix < screen.width; ix += PARALLEL_PIXELS_NUMBER)
        {
            mmxf_t x0 = (mmxf_t) ((ix + coord_x) / screen.zoom) + mandelbrot->dx;
            mmxi_t iterations = calc_mandelbrot_point_AVX2_overload_ops(x0, y0);

            mmxi_t colors = _mm256_i32gather_epi32((const int*) palette, iterations, sizeof(uint32_t));
            _mm256_store_si256((__m256i*)vmem_buffer, colors);

            vmem_buffer++;
        }
    }
    return NO_ERR;
}

// = AVX2 implementation =======================================================
static mmxi_t calc_mandelbrot_point_AVX2(const __m256 x0, const __m256 y0)
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
    RET_IF_ERR(mandelbrot, NULL_PTR_ERR);

    Screen     screen  = mandelbrot->screen;
    uint32_t* palette =  get_cur_palette(mandelbrot->palettes, mandelbrot->cur_palette);
    const float coord_x = screen.pos_x - screen.width / 2.0f;
    const float coord_y = screen.pos_y - screen.height / 2.0f;

    __m256i* vmem_buffer = (__m256i*) screen.graphic.surface->pixels;

    for (int iy = 0; iy < screen.height; iy++)
    {
        const __m256 y0 = _mm256_set1_ps(((float)iy + coord_y) / screen.zoom);

        for (int ix = 0; ix < screen.width; ix += PARALLEL_PIXELS_NUMBER)
        {
            __m256 x0 = _mm256_set1_ps(((float)ix + coord_x) / screen.zoom);
            x0 = _mm256_add_ps(x0, mandelbrot->dx);

            __m256i iterations = calc_mandelbrot_point_AVX2(x0, y0);

            __m256i colors = _mm256_i32gather_epi32((const int*) palette, iterations, sizeof(uint32_t));
            _mm256_store_si256(vmem_buffer, colors);

            vmem_buffer++;
        }
    }
    return NO_ERR;
}

// = Vectorized without SIMD implementation ====================================
#define PARALLEL_INSTRUCTION(...)                         \
    for (size_t i = 0; i < PARALLEL_PIXELS_NUMBER; i++) \
    {                                                   \
        __VA_ARGS__                                     \
    }

// TODO: consider this approach:
#define PARALLEL_INSTRUCTION_                         \
    for (size_t i = 0; i < PARALLEL_PIXELS_NUMBER; i++) \

// BAH: gcc unlike clang couldn't turn this into intrinsics you can write about it in the report
error_code calc_mandelbrot_vector(const Mandelbrot* mandelbrot)
{
    Screen     screen    = mandelbrot->screen;
    uint32_t* palette    =  get_cur_palette(mandelbrot->palettes, mandelbrot->cur_palette);
    const float coord_x  = screen.pos_x - screen.width / 2.0f;
    const float coord_y  = screen.pos_y - screen.height / 2.0f;

   uint32_t* vmem_buffer = (uint32_t*) screen.graphic.surface->pixels;

    // TODO: C89 (after hoisting it will be the same)
    float y0[PARALLEL_PIXELS_NUMBER]      = {};
    float x0[PARALLEL_PIXELS_NUMBER]      = {};
    float x[PARALLEL_PIXELS_NUMBER]       = {};
    float y[PARALLEL_PIXELS_NUMBER]       = {};
    float x2[PARALLEL_PIXELS_NUMBER]      = {};
    float y2[PARALLEL_PIXELS_NUMBER]      = {};
    float xy[PARALLEL_PIXELS_NUMBER]      = {};
    float radius2[PARALLEL_PIXELS_NUMBER] = {};
    int cmp[PARALLEL_PIXELS_NUMBER]       = {};

    for (int iy = 0; iy < screen.height; iy++)
    {
        PARALLEL_INSTRUCTION_{y0[i] =  (iy + coord_y) / screen.zoom;}

        for (int ix = 0; ix < screen.width; ix += PARALLEL_PIXELS_NUMBER)
        {
            PARALLEL_INSTRUCTION(x0[i]  = ((ix + coord_x + i) / screen.zoom);)

            PARALLEL_INSTRUCTION(x[i] = x0[i];)
            PARALLEL_INSTRUCTION(y[i] = y0[i];)

            int iterations[PARALLEL_PIXELS_NUMBER] = {};

            for (int iteration = 0; iteration < MAX_ITERATION_NUMBER; iteration++)
            {
                PARALLEL_INSTRUCTION(x2[i]      =  x[i] *  x[i];)
                PARALLEL_INSTRUCTION(y2[i]      =  y[i] *  y[i];)
                PARALLEL_INSTRUCTION(xy[i]      =  x[i] *  y[i];)
                PARALLEL_INSTRUCTION(radius2[i] = x2[i] + y2[i];)

                char mask = 0;
                PARALLEL_INSTRUCTION(mask |= (radius2[i] < MAX_RADIUS_2_VECT[i]) << (PARALLEL_PIXELS_NUMBER - i - 1);)
                if (!mask) break;

                // PARALLEL_INSTRUCTION(cmp[i] = radius2[i] < MAX_RADIUS_2_VECT[i];)
                // char mask = 0;
                // PARALLEL_INSTRUCTION(mask += cmp[i];)
                // if (!mask) break;

                PARALLEL_INSTRUCTION(x[i] = x0[i] + x2[i] - y2[i];)
                PARALLEL_INSTRUCTION(y[i] = y0[i] + xy[i] + xy[i];)

                // PARALLEL_INSTRUCTION(iterations[i] += cmp[i];)
                PARALLEL_INSTRUCTION(iterations[i] += (mask >> PARALLEL_PIXELS_NUMBER - i - 1) & 1;)
            }

            PARALLEL_INSTRUCTION(*(vmem_buffer + i) = *(palette + iterations[i]);)
            vmem_buffer += PARALLEL_PIXELS_NUMBER;
        }
    }
    return NO_ERR;
}

// You make me proud!
#undef PARALLEL_INSTRUCTION
