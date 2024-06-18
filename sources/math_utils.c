
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

list_t* add_list(list_t* l, int v) {
    list_t* nl = (list_t*) malloc(sizeof(list_t));
    nl->val = v;
    nl->next = l;
    return nl;
}

SDL_bool list_mem(list_t* l, int v) {
    while (l != NULL) {
        if (l->val == v) return SDL_TRUE;
        l = l->next;
    }
    return SDL_FALSE;
}

list_t* remove_list(list_t* l, int v) {
    if (l == NULL) return l;
    else if (l->val == v) {
        list_t* ln = l->next;
        free(l);
        return remove_list(ln, v);
    } else {
        list_t* ln = remove_list(l->next, v);
        l->next = ln;
        return l;
    }
}

int list_size(list_t* l) {
    int n = 0;
    while (l != NULL) {
        n++;
        l = l->next;
    }
    return n;
}