#include <time.h>
#include <stdio.h>

#include "mandelbrot.h"
#include "benchmark.h"

int benchmark_function(Mandelbrot* mandelbrot)
{
    size_t tics_amount = 0;
    clock_t tic_start = clock();
    for (size_t i = 0; i < FUNCTIONS_RUNS_NUMBER; i++)
    {
        mandelbrot->method.calculate(mandelbrot);
    }
    clock_t tic_end = clock();
    tics_amount = tic_end - tic_start;

    printf("Ticks: %zu\n", tics_amount);
    printf("Ticks per call: %lf\n", tics_amount / (float) FUNCTIONS_RUNS_NUMBER);

    return tics_amount;
};
