
#include "math_utils.h"
#include "camera.h"

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

float distance(triangle_t* t, camera_t* camera) {
    float dx = t->p1.x - camera->x;
    float dy = t->p1.y - camera->y;
    float dz = t->p1.z - camera->z;
    float dp1 = sqrt(dx*dx + dy*dy + dz*dz);

    dx = t->p2.x - camera->x;
    dy = t->p2.y - camera->y;
    dz = t->p2.z - camera->z;
    float dp2 = sqrt(dx*dx + dy*dy + dz*dz);

    dx = t->p3.x - camera->x;
    dy = t->p3.y - camera->y;
    dz = t->p3.z - camera->z;
    float dp3 = sqrt(dx*dx + dy*dy + dz*dz);
    return (dp1+dp2+dp3)/3;
}