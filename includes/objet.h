
#ifndef OBJET_H
#define OBJET_H

#include "math_utils.h"

typedef struct {
    int size;
    triangle_t** triangles;
} objet_t;

objet_t* load_obj_file(char* filename);

#endif