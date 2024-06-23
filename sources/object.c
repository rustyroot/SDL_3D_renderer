
#include "../includes/object.h"
#include "../includes/math_utils.h"

objet_t* load_obj_file(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Pas de fichier %s\n", filename);
    }
    int nb_sommet = 0;
    int nb_triangle = 0;
    int nb_vt = 0;
    int nb_vn = 0;
    char* c = malloc(sizeof(char)*200);
    while (fscanf(file, "%s", c) != EOF) {
        if (strcmp(c, "v") == 0) nb_sommet++;
        else if (strcmp(c, "f") == 0) nb_triangle++;
        else if (strcmp(c, "vt") == 0) nb_vt++;
        else if (strcmp(c, "vn") == 0) nb_vn++;
    }
    free(c);
    fclose(file);
    file = fopen(filename, "r");
    point_t* points = (point_t*) malloc(sizeof(point_t)*nb_sommet);
    triangle_t** triangles = (triangle_t**) malloc(sizeof(triangle_t*)*nb_triangle);
    char* c1 = malloc(sizeof(char)*200);
    int is = 0;
    int it = 0;

    while (fscanf(file, "%s", c1) != EOF) {
        if (strcmp(c1, "v") == 0) {
            float x,y,z;
            fscanf(file, "%f %f %f", &x, &y, &z);
            points[is].x = x;
            points[is].y = y;
            points[is].z = z;
            is++;
        } else if (strcmp(c1, "f") == 0) {
            int v1, vt1, vn1;
            int v2, vt2, vn2;
            int v3, vt3, vn3;
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
            triangles[it] = malloc(sizeof(triangle_t));
            triangles[it]->color.a = 255;
            triangles[it]->color.r = rand()%255;
            triangles[it]->color.g = rand()%255;
            triangles[it]->color.b = rand()%255;
            triangles[it]->p1 = points[v1-1];
            triangles[it]->p2 = points[v2-1];
            triangles[it]->p3 = points[v3-1];
            it++;
        } else {
            fscanf(file, "%[^\n]", c1);
        }
    }
    fclose(file);
    free(points);
    free(c1);
    objet_t* obj = (objet_t*) malloc(sizeof(objet_t));
    obj->size = nb_triangle;
    obj->triangles = triangles;
    return obj;
}

void move_obj(objet_t* objet, point_t* vecteur) {
    for (int k = 0; k < objet->size; k++) {
        objet->triangles[k]->p1 = somme_point(objet->triangles[k]->p1, *vecteur);
        objet->triangles[k]->p2 = somme_point(objet->triangles[k]->p2, *vecteur);
        objet->triangles[k]->p3 = somme_point(objet->triangles[k]->p3, *vecteur);
    }
}