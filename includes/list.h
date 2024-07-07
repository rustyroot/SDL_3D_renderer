#ifndef LIST_H
#define LIST_H

#include <SDL2/SDL.h>

struct int_list_s {
    int val;
    struct int_list_s* next;
};
typedef struct int_list_s int_list_t;

int_list_t* add_int_list(int_list_t* l, int v);
SDL_bool int_list_mem(int_list_t* l, int v);
int_list_t* remove_int_list(int_list_t* l, int v);
int int_list_size(int_list_t* l);

struct list_s {
    void* val;
    struct list_s* next;
};
typedef struct list_s list_t;

list_t* add_list(list_t* l, void* v);
SDL_bool list_mem(list_t* l, void* v);
list_t* remove_list(list_t* l, void* v);
int list_size(list_t* l);
void free_list(list_t* l, void* free_fnc(void* el));

#endif