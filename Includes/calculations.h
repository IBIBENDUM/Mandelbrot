#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#include "error_codes.h"

struct Mandelbrot;
typedef error_code (*calc_implement_func)(const Mandelbrot*);
enum Calc_implement
{
    CALC_PRIMITIVE,
    CALC_AVX2,
    CALC_AVX2_OVERLOAD,
    CALC_VECTORIZED,
};

#include <immintrin.h>
#include "mandelbrot_config.h"
const __m256      MAX_RADIUS_2_256     = _mm256_set1_ps(MAX_RADIUS2);
const __m256      DX_FACTOR            = _mm256_set_ps(7.0,  6.0,  5.0,  4.0, 3.0,  2.0,  1.0,  0.0);

error_code calc_mandelbrot_primitive(const Mandelbrot*);

error_code calc_mandelbrot_AVX2(const Mandelbrot*);

error_code calc_mandelbrot_AVX2_overload_ops(const Mandelbrot*);

error_code calc_mandelbrot_vector(const Mandelbrot*);

const calc_implement_func CALC_FUNCS[] =
{
    calc_mandelbrot_primitive,
    calc_mandelbrot_AVX2,
    calc_mandelbrot_AVX2_overload_ops,
    calc_mandelbrot_vector,
};
const int CALC_FUNCS_AMOUNT = sizeof(CALC_FUNCS) / sizeof(calc_implement_func);
const char* const METHODS_NAMES[CALC_FUNCS_AMOUNT] = {"Primitive", "AVX2", "AVX2 with overload", "Vectorized"};


#endif // CALCULATIONS_H_

