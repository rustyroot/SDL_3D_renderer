
#ifndef OBJECT_H
#define OBJECT_H

#include "math_utils.h"
#include "physic.h"

typedef struct {
    int nb_sommets;
    physical_point_t** sommets;
    int nb_triangles;
    int** triangles_indice;
    triangle_t** triangles;
    int nb_springs;
    int** spring_indice;
    spring_t** springs;
} objet_t;

objet_t* load_obj_file(char* filename);
void compute_spring(objet_t* obj, float k);
void move_obj(objet_t* objet, point_t* vecteur);
objet_t* copy_obj(objet_t* objet);
void free_obj(objet_t* objet);

#endif