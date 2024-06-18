
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <SDL2/SDL.h>

typedef struct {
    float x;
    float y;
    float z;
} point_t;

typedef struct {
    point_t p1, p2, p3;
    SDL_Color color;
} triangle_t;

struct list_s {
    int val;
    struct list_s* next;
};
typedef struct list_s list_t;

double to_degrees(double radians);
double to_radians(double degrees);
point_t produit_vectoriel(point_t p1, point_t p2);
float dot(point_t p1, point_t p2);
triangle_t* init_triangle(point_t p1, point_t p2, point_t p3, SDL_Color c);
point_t somme_point(point_t p1, point_t p2);
point_t produit_par_scalaire(float x, point_t p);
list_t* add_list(list_t* l, int v);
SDL_bool list_mem(list_t* l, int v);
list_t* remove_list(list_t* l, int v);
int list_size(list_t* l);

#endif