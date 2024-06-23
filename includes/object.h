
#ifndef OBJECT_H
#define OBJECT_H

#include "math_utils.h"

typedef struct {
    int size;
    triangle_t** triangles;
} objet_t;

objet_t* load_obj_file(char* filename);
void move_obj(objet_t* objet, point_t* vecteur);

#endif