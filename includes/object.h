
#ifndef OBJECT_H
#define OBJECT_H

#include "list.h"
#include "math_utils.h"
#include "physic.h"
#include <SDL2/SDL_stdinc.h>

typedef struct {
    int nb_sommets;
    physical_point_t** sommets;

    int nb_triangles;
    int** triangles_indice;
    triangle_t** triangles;

    int nb_springs;
    int** spring_indice;
    spring_t** springs;

    list_t** sommets_triangles;  // tableau de liste. La liste i contient les triangles dont un des sommets est le sommet i.
} objet_t;

objet_t* load_obj_file(char* filename);
void compute_spring(objet_t* obj, float k);
void move_obj(objet_t* objet, point_t* vecteur);
objet_t* copy_obj(objet_t* objet);
void free_obj(objet_t* objet);

triangle_t* get_nearest_triangle(point_t* sommet, objet_t* obj);
SDL_bool collision_sommet_objet(point_t* sommet, objet_t* obj);
void collision(objet_t* obj, objet_t** objs, int nb_objets);

#endif