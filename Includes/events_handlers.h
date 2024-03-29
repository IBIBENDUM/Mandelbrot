#ifndef EVENTS_HANDLERS_H_
#define EVENTS_HANDLERS_H_

error_code keyboard_handler(SDL_Event* event, Mandelbrot* mandelbrot);

error_code scroll_handler(SDL_Event* event, Mandelbrot* screen);

error_code movement_handler(SDL_Event* event, Screen* screen);

#endif // EVENTS_HANDLERS_H_
