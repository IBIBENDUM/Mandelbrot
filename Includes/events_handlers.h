#ifndef EVENTS_HANDLERS_H_
#define EVENTS_HANDLERS_H_

int keyboard_handler(SDL_Event* event, Mandelbrot* mandelbrot);

int scroll_handler(SDL_Event* event, Screen* screen);

#endif // EVENTS_HANDLERS_H_
