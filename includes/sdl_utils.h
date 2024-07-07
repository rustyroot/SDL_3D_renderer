#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include "SDL2/SDL.h"
#include "list.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

void poll_event (SDL_Event* event, list_t** keyDown, SDL_bool* is_running_ptr, float* mouse_events);
SDL_Surface** create_surfaces_from_text(TTF_Font* font, const char* text, SDL_Color color, int* nombre_surfaces);
SDL_Rect* create_rect_from_text(TTF_Font* font, const char* text, int nombre_ligne);

#endif