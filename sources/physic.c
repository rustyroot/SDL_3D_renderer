#include "../includes/physic.h"
#include <SDL2/SDL_timer.h>

const float gravitational_constant = 6.67430 * 0.000000000001;

physical_point_t* create_physical_point (point_t *position, float charge, float mass) {
    physical_point_t* point = malloc(sizeof(physical_point_t));
    point->position = position;
    point->speed = malloc_point((point_t){0, 0, 0});
    point->acceleration = malloc_point((point_t){0, 0, 0});
    point->charge = charge;
    point->mass = mass;
    point->time = SDL_GetTicks64();
    return point;
}

void destroy_physical_point (physical_point_t* point) {
    free(point->position);
    free(point->speed);
    free(point->acceleration);
    free(point);
}

void update_physical_point (physical_point_t* point, point_t* forces, int nb_forces) {
    Uint64 t = SDL_GetTicks64(); // ms
    float time_since_last_update = (float)(t - point->time)/1000; // s
    for (int i = 0; i < nb_forces; i++) {
        copy_point(somme_point(*point->acceleration, produit_par_scalaire(1/point->mass, forces[i])), point->acceleration);
    }
    copy_point(somme_point(*point->speed, produit_par_scalaire(time_since_last_update, *point->acceleration)), point->speed);
    copy_point(somme_point(*point->position, produit_par_scalaire(time_since_last_update, *point->speed)), point->position);
}

point_t gravitational_force (physical_point_t point1, physical_point_t point2) {
    // return Fg 1->2
    float r = distance(*point1.position, *point2.position);
    float force_norm = (gravitational_constant * point1.mass * point2.mass)/(r *r);
    point_t vector12 = {
        point2.position->x - point1.position->x,
        point2.position->y - point1.position->y,
        point2.position->z - point1.position->z
    };
    float norm_vector12 = norm(vector12);
    point_t force = {
        vector12.x * force_norm / norm_vector12,
        vector12.y * force_norm / norm_vector12,
        vector12.z * force_norm / norm_vector12
    };
    return force;
}