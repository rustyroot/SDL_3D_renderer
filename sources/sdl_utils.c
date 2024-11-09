#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include "sdl_utils.h"

void poll_event (SDL_Event* event, list_t** keyDown, SDL_bool* is_running_ptr, float* mouse_events) {
    while (SDL_PollEvent(event)) {
            switch (event->type) {
                case SDL_KEYDOWN:
                    switch (event->key.keysym.sym) {
                        case SDLK_ESCAPE :
                            *is_running_ptr = SDL_FALSE;
                            break;
                        default:
                            if (!list_mem(*keyDown, event->key.keysym.sym)) *keyDown = add_list(*keyDown, event->key.keysym.sym);
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    *keyDown = remove_list(*keyDown, event->key.keysym.sym);
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

SDL_Surface** create_surfaces_from_text(TTF_Font* font, const char* text, SDL_Color color, int* nombre_surfaces) {
    *nombre_surfaces = 1;

    int text_size = strlen(text);
    char* copy_text = (char*) malloc(sizeof(char)*(text_size+1));
    strcpy(copy_text, text);
    for (int i=0; i<text_size; i++) {
        if (copy_text[i] == '\n') {
            copy_text[i] = 0;
            (*nombre_surfaces)++;
        }
    }

    SDL_Surface** s = malloc(sizeof(SDL_Surface*)**nombre_surfaces);
    char* ligne = copy_text;
    for (int i=0; i<*nombre_surfaces; i++) {
        s[i] = TTF_RenderUTF8_Solid(font, ligne, color);
        ligne = ligne + strlen(ligne) + 1;
    }

    free(copy_text);
    return s;
}

SDL_Rect* create_rect_from_text(TTF_Font* font, const char* text, int nombre_ligne) {
    int text_size = strlen(text);
    char* copy_text = (char*) malloc(sizeof(char)*(text_size+1));
    strcpy(copy_text, text);
    for (int i=0; i<text_size; i++) {
        if (copy_text[i] == '\n') copy_text[i] = 0;
    }
    SDL_Rect* rect = (SDL_Rect*) malloc(sizeof(SDL_Rect)*nombre_ligne);
    char* ligne = copy_text;
    for (int i=0; i<nombre_ligne; i++) {
        TTF_SizeText(font, ligne, &rect[i].w, &rect[i].h);
        rect[i].x = 0;
        rect[i].y = 30*(i+1);
        ligne = ligne + strlen(ligne) + 1;
    }
    free(copy_text);
    return rect;
}
