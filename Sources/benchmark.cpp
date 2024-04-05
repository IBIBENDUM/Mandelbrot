#include <time.h>
#include <stdio.h>

#include "mandelbrot.h"
#include "benchmark.h"

int benchmark_function(Mandelbrot* mandelbrot)
{
    printf("Calculation method: %s\n", METHODS_NAMES[mandelbrot->method.id]);
    size_t ticks_amount_total = 0;
    for (size_t i = 0; i < FUNCTIONS_RUNS_NUMBER; i++)
    {
        clock_t tic_start = clock();
        mandelbrot->method.calculate(mandelbrot);
        clock_t tic_end = clock();
        size_t ticks_amount = tic_end - tic_start;
        ticks_amount_total += ticks_amount;
        printf("%zu ", ticks_amount);
    }
    printf("\nTicks amount total: %zu\n", ticks_amount_total);

    return ticks_amount_total;
};
