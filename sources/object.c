
#include "../includes/object.h"
#include "../includes/math_utils.h"
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>

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

    physical_point_t** points = (physical_point_t**) malloc(sizeof(physical_point_t*)*nb_sommet);
    triangle_t** triangles = (triangle_t**) malloc(sizeof(triangle_t*)*nb_triangle);

    int** triangle_indice = (int**) malloc(sizeof(int*)*nb_triangle);
    char* c1 = malloc(sizeof(char)*200);
    int is = 0;
    int it = 0;

    list_t** sommets_triangles = (list_t**) malloc(sizeof(list_t*)*nb_sommet);
    for (int i=0; i<nb_sommet; i++) sommets_triangles[i] = NULL;

    while (fscanf(file, "%s", c1) != EOF) {
        if (strcmp(c1, "v") == 0) {
            float x,y,z;
            fscanf(file, "%f %f %f", &x, &y, &z);
            point_t pos = {x,y,z};
            points[is] = create_physical_point(malloc_point(pos), 0, 1);
            is++;
        } else if (strcmp(c1, "f") == 0) {
            int v1, vt1, vn1;
            int v2, vt2, vn2;
            int v3, vt3, vn3;
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
            triangle_indice[it] = malloc(sizeof(int)*3);
            triangle_indice[it][0] = v1-1;
            triangle_indice[it][1] = v2-1;
            triangle_indice[it][2] = v3-1;

            triangles[it] = malloc(sizeof(triangle_t));
            triangles[it]->color.a = 255;
            triangles[it]->color.r = rand()%255;
            triangles[it]->color.g = rand()%255;
            triangles[it]->color.b = rand()%255;
            triangles[it]->p1 = points[triangle_indice[it][0]]->position;
            triangles[it]->p2 = points[triangle_indice[it][1]]->position;
            triangles[it]->p3 = points[triangle_indice[it][2]]->position;


            sommets_triangles[v1-1] = add_list(sommets_triangles[v1-1], triangles[it]);
            sommets_triangles[v2-1] = add_list(sommets_triangles[v2-1], triangles[it]);
            sommets_triangles[v3-1] = add_list(sommets_triangles[v3-1], triangles[it]);

            it++;
        } else {
            fscanf(file, "%[^\n]", c1);
        }
    }
    fclose(file);

    int nb_springs = 0;
    for (int i=0; i<nb_sommet; i++) {
        for (int j=i+1; j<nb_sommet; j++) {
            if (distance(*points[i]->position, *points[j]->position) < 0.5) nb_springs++;
        }
    }

    //int nb_springs = nb_sommet*(nb_sommet-1)/2;
    spring_t** springs = (spring_t**) malloc(sizeof(spring_t*)*nb_springs);
    int** spring_indice = (int**) malloc(sizeof(int*)*nb_springs);
    int spring_i = 0;
    for (int i=0; i<nb_sommet; i++) {
        for (int j=i+1; j<nb_sommet; j++) {
            if (distance(*points[i]->position, *points[j]->position) < 0.5) {
                spring_indice[spring_i] = malloc(sizeof(int)*2);
                spring_indice[spring_i][0] = i;
                spring_indice[spring_i][1] = j;

                springs[spring_i] = malloc(sizeof(spring_t));
                springs[spring_i]->k = 1;
                springs[spring_i]->p1 = points[spring_indice[spring_i][0]];
                springs[spring_i]->p2 = points[spring_indice[spring_i][1]];
                springs[spring_i]->size = distance(*springs[spring_i]->p1->position, *springs[spring_i]->p2->position);
                spring_i++;
            }
        }
    }

    free(c1);
    objet_t* obj = (objet_t*) malloc(sizeof(objet_t));
    obj->nb_sommets = nb_sommet;
    obj->sommets = points;
    obj->nb_triangles = nb_triangle;
    obj->triangles_indice = triangle_indice;
    obj->triangles = triangles;
    obj->nb_springs = nb_springs;
    obj->spring_indice = spring_indice;
    obj->springs = springs;
    obj->sommets_triangles = sommets_triangles;
    return obj;
}

void compute_spring(objet_t* obj, float k) {
    for (int i=0; i<obj->nb_springs; i++) {
        free(obj->spring_indice[i]);
        free(obj->springs[i]);
    }
    free(obj->spring_indice);
    free(obj->springs);

    float seuil = 20.5;

    int nb_springs = 0;
    for (int i=0; i<obj->nb_sommets; i++) {
        for (int j=i+1; j<obj->nb_sommets; j++) {
            if (distance(*obj->sommets[i]->position, *obj->sommets[j]->position) < seuil) nb_springs++;
        }
    }

    //int nb_springs = obj->nb_sommets*(obj->nb_sommets-1)/2;
    spring_t** springs = (spring_t**) malloc(sizeof(spring_t*)*nb_springs);
    int** spring_indice = (int**) malloc(sizeof(int*)*nb_springs);
    int spring_i = 0;
    for (int i=0; i<obj->nb_sommets; i++) {
        for (int j=i+1; j<obj->nb_sommets; j++) {
            if (distance(*obj->sommets[i]->position, *obj->sommets[j]->position) < seuil) {
                spring_indice[spring_i] = malloc(sizeof(int)*2);
                spring_indice[spring_i][0] = i;
                spring_indice[spring_i][1] = j;

                springs[spring_i] = malloc(sizeof(spring_t));
                springs[spring_i]->k = k;///distance(*obj->sommets[i]->position, *obj->sommets[j]->position);
                springs[spring_i]->p1 = obj->sommets[spring_indice[spring_i][0]];
                springs[spring_i]->p2 = obj->sommets[spring_indice[spring_i][1]];
                springs[spring_i]->size = distance(*springs[spring_i]->p1->position, *springs[spring_i]->p2->position);
                spring_i++;
            }
        }
    }
    obj->nb_springs = nb_springs;
    obj->spring_indice = spring_indice;
    obj->springs = springs;
}

void move_obj(objet_t* objet, point_t* vecteur) {
    for (int i=0; i<objet->nb_sommets; i++) copy_point(somme_point(*objet->sommets[i]->position, *vecteur), objet->sommets[i]->position);
}

objet_t* copy_obj(objet_t* objet) {
    objet_t* obj = (objet_t*) malloc(sizeof(objet_t));
    obj->nb_sommets = objet->nb_sommets;
    obj->sommets = (physical_point_t**) malloc(sizeof(physical_point_t*)*obj->nb_sommets);
    for (int i=0; i<obj->nb_sommets; i++) {
        obj->sommets[i] = create_physical_point(malloc_point(*objet->sommets[i]->position), objet->sommets[i]->charge, objet->sommets[i]->mass);
    }

    obj->nb_triangles = objet->nb_triangles;
    obj->triangles_indice = (int**) malloc(sizeof(int*)*obj->nb_triangles);
    for (int i=0; i<obj->nb_triangles; i++) {
        obj->triangles_indice[i] = malloc(sizeof(int)*3);
        for (int j=0; j<3; j++) obj->triangles_indice[i][j] = objet->triangles_indice[i][j];
    }
    obj->triangles = (triangle_t**) malloc(sizeof(triangle_t*)*obj->nb_triangles);
    obj->sommets_triangles = (list_t**) malloc(sizeof(list_t*)*obj->nb_sommets);
    for (int i=0; i<obj->nb_triangles; i++) {
        obj->triangles[i] = (triangle_t*) malloc(sizeof(triangle_t));
        obj->triangles[i]->color = objet->triangles[i]->color;
        obj->triangles[i]->p1 = obj->sommets[obj->triangles_indice[i][0]]->position;
        obj->triangles[i]->p2 = obj->sommets[obj->triangles_indice[i][1]]->position;
        obj->triangles[i]->p3 = obj->sommets[obj->triangles_indice[i][2]]->position;

        obj->sommets_triangles[obj->triangles_indice[i][0]] = add_list(obj->sommets_triangles[obj->triangles_indice[i][0]], obj->triangles[i]);
        obj->sommets_triangles[obj->triangles_indice[i][1]] = add_list(obj->sommets_triangles[obj->triangles_indice[i][1]], obj->triangles[i]);
        obj->sommets_triangles[obj->triangles_indice[i][2]] = add_list(obj->sommets_triangles[obj->triangles_indice[i][2]], obj->triangles[i]);
    }

    obj->nb_springs = objet->nb_springs;
    obj->spring_indice = (int**) malloc(sizeof(int*)*obj->nb_springs);
    for (int i=0; i<obj->nb_springs; i++) {
        obj->spring_indice[i] = malloc(sizeof(int)*2);
        obj->spring_indice[i][0] = objet->spring_indice[i][0];
        obj->spring_indice[i][1] = objet->spring_indice[i][1];
    }
    obj->springs = (spring_t**) malloc(sizeof(spring_t*)*obj->nb_springs);
    for (int i=0; i<obj->nb_springs; i++) {
        obj->springs[i] = (spring_t*) malloc(sizeof(spring_t));
        obj->springs[i]->k = objet->springs[i]->k;
        obj->springs[i]->p1 = obj->sommets[obj->spring_indice[i][0]];
        obj->springs[i]->p2 = obj->sommets[obj->spring_indice[i][1]];
        obj->springs[i]->size = objet->springs[i]->size;
    }
    return obj;
}

void free_obj(objet_t* objet) {
    for (int i=0; i<objet->nb_sommets; i++) {
        destroy_physical_point(objet->sommets[i]);
    }
    free(objet->sommets);
    for (int i=0; i<objet->nb_triangles; i++) {
        free(objet->triangles_indice[i]);
        free(objet->triangles[i]);
    }
    free(objet->triangles_indice);
    free(objet->triangles);
    if (objet->springs != NULL) {
        for (int i=0; i<objet->nb_springs; i++) {
            free(objet->spring_indice[i]);
            free(objet->springs[i]);
        }
        free(objet->spring_indice);
        free(objet->springs);
    }
    for (int i=0; i<objet->nb_sommets; i++) free_list(objet->sommets_triangles[i], NULL);
    free(objet);
}

triangle_t* get_nearest_triangle(point_t* sommet, objet_t* obj) {
    triangle_t* t = obj->triangles[0];
    int dist = distance_triangle_point(*t, *sommet);
    for (int i=1; i<obj->nb_triangles; i++) {
        int d = distance_triangle_point(*obj->triangles[i], *sommet);
        if (d < dist) {
            dist = d;
            t = obj->triangles[i];
        }
    }
    return t;
}

SDL_bool collision_sommet_objet(point_t* sommet, objet_t* obj) {
    int nb = 0;
    for (int i=0; i<obj->nb_triangles; i++) {
        point_t* O = obj->triangles[i]->p1;
        point_t SO = {O->x - sommet->x, O->y - sommet->y, O->z - sommet->z};
        if (SO.y <= 0) continue;
        float y1 = asin(SO.z / sqrt(SO.y*SO.y + SO.z*SO.z));
        float x1 = asin(SO.x / sqrt(SO.x*SO.x + SO.y*SO.y));

        point_t* O2 = obj->triangles[i]->p2;
        point_t SO2 = {O2->x - sommet->x, O2->y - sommet->y, O2->z - sommet->z};
        if (SO2.y <= 0) continue;
        float y2 = asin(SO2.z / sqrt(SO2.y*SO2.y + SO2.z*SO2.z));
        float x2 = asin(SO2.x / sqrt(SO2.x*SO2.x + SO2.y*SO2.y));

        point_t* O3 = obj->triangles[i]->p3;
        point_t SO3 = {O3->x - sommet->x, O3->y - sommet->y, O3->z - sommet->z};
        if (SO3.y <= 0) continue;
        float y3 = asin(SO3.z / sqrt(SO3.y*SO3.y + SO3.z*SO3.z));
        float x3 = asin(SO3.x / sqrt(SO3.x*SO3.x + SO3.y*SO3.y));

        float areaTriangle = abs_float((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1));
        float sum = abs_float((x1)*(y2) - (x2)*(y1)) + abs_float((x2)*(y3) - (x3)*(y2)) + abs_float((x3)*(y1) - (x1)*(y3));
        if (sum > areaTriangle-0.000001 && sum < areaTriangle+0.000001) {
            nb++;
        }
    }

    return nb%2 == 1;
}

void collision(objet_t* obj, objet_t** objs, int nb_objets) {
    for (int i=0; i<obj->nb_sommets; i++) {
        for (int j=1; j<nb_objets; j++) {
            if (obj != objs[j] && collision_sommet_objet(obj->sommets[i]->position, objs[j])) {
                // sommet i à l'interieure de l'objet j
                triangle_t* triangle = get_nearest_triangle(obj->sommets[i]->position, objs[j]);
                point_t normale_triangle = produit_vectoriel(soustraction_point(*triangle->p1, *triangle->p2), soustraction_point(*triangle->p3, *triangle->p2));
                int dir = dot(normale_triangle, soustraction_point(*obj->sommets[i]->position, *triangle->p1))>0 ? -1 : 1;
                normale_triangle = produit_par_scalaire(dir/norm(normale_triangle), normale_triangle);
                float distance = distance_triangle_point(*triangle, *obj->sommets[i]->position);

                *obj->sommets[i]->position = somme_point(*obj->sommets[i]->position, produit_par_scalaire(distance, normale_triangle));
                printf("nd : %f, od : %f\n", distance_triangle_point(*triangle, *obj->sommets[i]->position), distance);
                // *triangle->p1 = somme_point(*triangle->p1, produit_par_scalaire(-distance/2, normale_triangle));
                // *triangle->p2 = somme_point(*triangle->p2, produit_par_scalaire(-distance/2, normale_triangle));
                // *triangle->p3 = somme_point(*triangle->p3, produit_par_scalaire(-distance/2, normale_triangle));
            }
        }
    }
}