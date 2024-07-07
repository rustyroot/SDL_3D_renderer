
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
    t->p1 = malloc_point(p1);
    t->p2 = malloc_point(p2);
    t->p3 = malloc_point(p3);
    return t;
}

point_t somme_point(point_t p1, point_t p2) {
    point_t s = {p1.x+p2.x, p1.y+p2.y, p1.z+p2.z};
    return s;
}

point_t soustraction_point(point_t p1, point_t p2) {
    point_t s = {p1.x - p2.x, p1.y-p2.y, p1.z-p2.z};
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

float abs_float(float a) {
    if (a>0) return a;
    else return -a;
}

float min_float(float a, float b) {
    if (a<b) return a;
    else return b;
}

float max_float(float a, float b) {
    if (a<b) return b;
    else return a;
}

point_t* malloc_point(point_t p) {
    point_t* mp = malloc(sizeof(point_t));
    mp->x = p.x;
    mp->y = p.y;
    mp->z = p.z;
    return mp;
}

void copy_point(point_t src, point_t* dst) {
    dst->x = src.x;
    dst->y = src.y;
    dst->z = src.z;
}

float distance_triangle_point(triangle_t triangle, point_t point) {
    point_t normale_triangle = produit_vectoriel(soustraction_point(*triangle.p1, *triangle.p2), soustraction_point(*triangle.p3, *triangle.p2));
    int dir = dot(normale_triangle, soustraction_point(point, *triangle.p1))>0 ? -1 : 1;
    normale_triangle = produit_par_scalaire(dir/norm(normale_triangle), normale_triangle);
    float d = abs_float(dot(soustraction_point(*triangle.p1, point), normale_triangle));

    point_t pp = somme_point(point, produit_par_scalaire(d, normale_triangle)); // projeter de point sur le plan du triangle
    pp = soustraction_point(pp, *triangle.p1);

    point_t v1 = soustraction_point(*triangle.p2, *triangle.p1);
    point_t v2 = soustraction_point(*triangle.p3, *triangle.p1);
    point_t v3 = produit_vectoriel(v1, v2);

    point_t v1_inv = {v2.y*v3.z-v3.y*v2.z, v3.x*v2.z-v2.x*v3.z, v2.x*v3.y-v3.x*v2.y};
    point_t v2_inv = {v3.y*v1.z-v1.y*v3.z, v1.x*v3.z-v3.x*v1.z, v3.x*v1.y-v1.x*v3.y};
    point_t v3_inv = {v1.y*v2.z-v2.y*v1.z, v2.x*v1.z-v1.x*v2.z, v1.x*v2.y-v2.x*v1.y};

    point_t pp_nouvelle_base = {
        v1_inv.x*pp.x + v1_inv.y*pp.y + v1_inv.z*pp.z, 
        v2_inv.x*pp.x + v2_inv.y*pp.y + v2_inv.z*pp.z, 
        v3_inv.x*pp.x + v3_inv.y*pp.y + v3_inv.z*pp.z
    };

    point_t point_triangle = {0,0,0};
    if (pp_nouvelle_base.x < 0) {
        point_triangle.y = max_float(0, min_float(pp_nouvelle_base.y, 1));
    } else if (pp_nouvelle_base.y < 0) {
        point_triangle.x = max_float(0, min_float(pp_nouvelle_base.x, 1));
    } else if (pp_nouvelle_base.y < 1 - pp_nouvelle_base.x) {
        point_triangle = pp_nouvelle_base;
    } else {
        float position_sur_ligne = max_float(0, min_float((-pp_nouvelle_base.x+1+pp_nouvelle_base.y)/2, 1));
        point_triangle.x = 1-position_sur_ligne;
        point_triangle.y = position_sur_ligne;
    }

    point_triangle.x = v1.x*point_triangle.x + v2.x*point_triangle.y + v3.x*point_triangle.z;
    point_triangle.y = v1.y*point_triangle.x + v2.y*point_triangle.y + v3.y*point_triangle.z;
    point_triangle.z = v1.z*point_triangle.x + v2.z*point_triangle.y + v3.z*point_triangle.z;

    point_triangle = somme_point(point_triangle, *triangle.p1);

    float distance_totale = distance(point_triangle, point);

    return distance_totale;
}