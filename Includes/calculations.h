#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#include "error_codes.h"

struct Mandelbrot;
typedef error_code (*Mandelbrot_calculation_function)(const Mandelbrot*);

enum Mandelbrot_calculation_method
{
    METHOD_SIMPLE,
    METHOD_VECTORIZED,
    METHOD_AVX2,
    METHOD_AVX2_WITH_OVERLOADED_OPERATORS
};

#include "mandelbrot_config.h"

error_code calc_mandelbrot_primitive(const Mandelbrot*);

error_code calc_mandelbrot_AVX2(const Mandelbrot*);

error_code calc_mandelbrot_AVX2_with_overloaded_operators(const Mandelbrot*);

error_code calc_mandelbrot_vectorized(const Mandelbrot*);

const Mandelbrot_calculation_function CALCULATION_FUNCTIONS[] = {
    calc_mandelbrot_primitive,
    calc_mandelbrot_vectorized,
    calc_mandelbrot_AVX2,
    calc_mandelbrot_AVX2_with_overloaded_operators,
};

const int CALCULATION_FUNCTIONS_AMOUNT = sizeof(CALCULATION_FUNCTIONS) / sizeof(Mandelbrot_calculation_function);
const char* const METHODS_NAMES[CALCULATION_FUNCTIONS_AMOUNT] = { "Simple", "Vectorized", "AVX2", "AVX2 with overloaded operators" };

#endif // CALCULATIONS_H_

