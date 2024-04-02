#include "mandelbrot.h"

int main()
{
    Mandelbrot mandelbrot = {};
    init_mandelbrot(&mandelbrot);

    RETURN_IF_NULL(&mandelbrot, NULL_PTR_ERR);

    while (mandelbrot.is_running)
    {
        if (process_mandelbrot(&mandelbrot))
            mandelbrot.is_running = false;
    }

    destruct_mandelbrot(&mandelbrot);

    return 0;
}
