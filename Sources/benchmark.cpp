#include <time.h>
#include <stdio.h>

#include "mandelbrot.h"
#include "benchmark.h"

int benchmark_function(Mandelbrot* mandelbrot)
{
    size_t tics_amount = 0;
    for (size_t i = 0; i < FUNCTIONS_RUNS_NUMBER; i++)
    {
        clock_t tic_start = clock();
        mandelbrot->calc_func(mandelbrot);
        clock_t tic_end = clock();
        tics_amount += tic_end - tic_start;
    }

    printf("Ticks: %lf\n", tics_amount / (float) FUNCTIONS_RUNS_NUMBER);

    return tics_amount;
};
