#ifndef LIST_H
#define LIST_H

#include <SDL2/SDL.h>

struct list_s {
    int val;
    struct list_s* next;
};
typedef struct list_s list_t;

list_t* add_list(list_t* l, int v);
SDL_bool list_mem(list_t* l, int v);
list_t* remove_list(list_t* l, int v);
int list_size(list_t* l);

#endif