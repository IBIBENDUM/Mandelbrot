#include "mandelbrot.h"

int main()
{
    Mandelbrot mandelbrot = {};
    init_mandelbrot(&mandelbrot);

    RET_IF_ERR(&mandelbrot, NULL_PTR_ERR); // TODO: Is 0 considered an error? (naming)

    // TODO: WHY
    // SDL_Renderer* renderer = mandelbrot.screen.graphic.renderer;
    // SDL_Surface*  surface  = mandelbrot.screen.graphic.surface;

    while (mandelbrot.is_running)
    {
        if (!process_mandelbrot(&mandelbrot))
            mandelbrot.is_running = false;
    }

    destruct_mandelbrot(&mandelbrot);

    return 0;
}
