#ifndef MANDELBROT_CONFIG_H_
#define MANDELBROT_CONFIG_H_

const char* const WINDOW_TITLE         = "mandelbrot!";
const int         WINDOW_WIDTH         = 600;
const int         WINDOW_HEIGHT        = 600;

const float       DEFAULT_ZOOM         = 300.0;
const float       ZOOM_FACTOR          = 10.0;
const int         KBRD_COORD_STEP      = 20;

const size_t      MAX_ITERATION_NUMBER = 256;
const float       MAX_RADIUS2          = 100;

const __m256      MAX_RADIUS_2_256     = _mm256_set1_ps(MAX_RADIUS2);
const __m256      DX_FACTOR            = _mm256_set_ps(7.0,  6.0,  5.0,  4.0, 3.0,  2.0,  1.0,  0.0);

#endif // MANDELBROT_CONFIG_H_
