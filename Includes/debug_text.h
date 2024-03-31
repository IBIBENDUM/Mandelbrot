#ifndef DEBUG_TEXT_H_
#define DEBUG_TEXT_H_

error_code draw_debug_text(Mandelbrot* mandelbrot);

error_code draw_benchmark_text(Mandelbrot* mandelbrot);

error_code draw_benchmark_results(Mandelbrot* mandelbrot, int tics_amount);

#endif // DEBUG_TEXT_H
