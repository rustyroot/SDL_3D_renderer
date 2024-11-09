#ifndef PHYSIC_H
#define PHYSIC_H

#include "math_utils.h"
#include <SDL2/SDL.h>
#include <pthread.h>

typedef struct {
    point_t *position;
    point_t *speed;
    point_t *acceleration;
    Uint64 time;
    float charge;
    float mass;
    SDL_bool fixe;
    pthread_mutex_t mutex;
} physical_point_t;

typedef struct {
    physical_point_t *p1, *p2;
    float k;
    float size;
} spring_t;

// G = 6,67430(15) × 10−11 m³ kg⁻¹ s⁻²
physical_point_t* create_physical_point (point_t *position, float charge, float mass);
void destroy_physical_point (physical_point_t* point);
void update_physical_point (physical_point_t* point, point_t* forces, int nb_forces);
point_t gravitational_force (physical_point_t point1, physical_point_t point2);
void *force_ressort(void* argv);


#endif