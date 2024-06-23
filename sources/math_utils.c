
#include "../includes/math_utils.h"
#include <SDL2/SDL_stdinc.h>

double to_degrees(double radians) {
    return radians * (180.0 / M_PI);
}

double to_radians(double degrees) {
    return degrees * (M_PI / 180.0);
}

point_t produit_vectoriel(point_t p1, point_t p2) {
    point_t p = {p1.y*p2.z - p1.z*p2.y, p1.z*p2.x - p1.x*p2.z, p1.x*p2.y - p1.y*p2.x};
    return p;
}

float dot(point_t p1, point_t p2) {
    return p1.x*p2.x + p1.y*p2.y + p1.z*p2.z;
}

triangle_t* init_triangle(point_t p1, point_t p2, point_t p3, SDL_Color c) {
    triangle_t* t = malloc(sizeof(triangle_t));
    t->color = c;
    t->p1 = p1;
    t->p2 = p2;
    t->p3 = p3;
    return t;
}

point_t somme_point(point_t p1, point_t p2) {
    point_t s = {p1.x+p2.x, p1.y+p2.y, p1.z+p2.z};
    return s;
}

point_t produit_par_scalaire(float x, point_t p) {
    point_t s = {x*p.x, x*p.y, x*p.z};
    return s;
}

float distance(point_t point1, point_t point2) {
    float dx = point1.x - point2.x;
    float dy = point1.y - point2.y;
    float dz = point1.z - point2.z;
    return SDL_sqrtf(dx*dx + dy*dy + dz*dz);
}

float norm (point_t vector) {
    return SDL_sqrtf(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}