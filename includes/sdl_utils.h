#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include "SDL2/SDL.h"
#include "list.h"

void poll_event (SDL_Event* event, int_list_t** keyDown, SDL_bool* is_running_ptr, float* mouse_events);

#endif