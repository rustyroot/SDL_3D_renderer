#include "../includes/sdl_utils.h"
#include <SDL2/SDL_events.h>

void poll_event (SDL_Event* event, int_list_t** keyDown, SDL_bool* is_running_ptr, float* mouse_events) {
    while (SDL_PollEvent(event)) {
            switch (event->type) {
                case SDL_KEYDOWN:
                    switch (event->key.keysym.sym) {
                        case SDLK_ESCAPE :
                            *is_running_ptr = SDL_FALSE;
                            break;
                        default:
                            if (!int_list_mem(*keyDown, event->key.keysym.sym)) *keyDown = add_int_list(*keyDown, event->key.keysym.sym);
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    *keyDown = remove_int_list(*keyDown, event->key.keysym.sym);
                    break;

                case SDL_MOUSEWHEEL:
                    mouse_events[0] = event->wheel.preciseY;
                    break;

                case SDL_MOUSEMOTION:
                    mouse_events[1] = event->motion.xrel;
                    mouse_events[2] = event->motion.yrel;
                    break;

                case SDL_QUIT:
                    *is_running_ptr = SDL_FALSE;
                    break;
            }
        }
}