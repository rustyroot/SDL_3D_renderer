#include "../includes/camera.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>

camera_t* create_camera (point_t* position, float keyboard_sensitivity, float mouse_sensitivity, float fov) {
    camera_t* camera = malloc(sizeof(camera_t));
    camera->point = create_physical_point(position, 0, 0);
    camera->yaw = 0; // rotation à l'horizon pour invariant <--> radian
    camera->pitch = 0; // rotation à la verticale pour invariant /\ \/ radian
    camera->roll = 0 ; // rotation radian
    camera->keyboard_sensitivity = keyboard_sensitivity;
    camera->mouse_sensitivity = mouse_sensitivity;
    camera->fov = fov;
    return camera;
}

void destroy_camera(camera_t *camera) {
    destroy_physical_point(camera->point);
    free(camera);
}

void angles_to_screen_coordinates (float theta, float phi, float fov, SDL_Window* window, float* x, float* y) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    *x = (theta / fov) * w + 0.5*w;
    *y = -(phi / fov) * w + 0.5*w;
}

SDL_bool get_angles (camera_t* C, point_t* O, float* theta, float* phi) {
    point_t CP_1 = {cos(C->yaw), sin(C->yaw), 0};
    point_t CP_2 = {sin(-C->yaw) * cos(C->pitch), cos(C->pitch) * cos(C->yaw), sin(C->pitch)};
    point_t CP_3 = produit_vectoriel(CP_1, CP_2);
    
    point_t CO = {O->x - C->point->position->x, O->y - C->point->position->y, O->z - C->point->position->z};

    point_t CO_base_camera = {
        CP_1.x*CO.x + CP_1.y*CO.y + CP_1.z*CO.z,
        CP_2.x*CO.x + CP_2.y*CO.y + CP_2.z*CO.z,
        CP_3.x*CO.x + CP_3.y*CO.y + CP_3.z*CO.z,
    };

    *phi = asin(CO_base_camera.z / sqrt(CO_base_camera.y*CO_base_camera.y + CO_base_camera.z*CO_base_camera.z));
    *theta = asin(CO_base_camera.x / sqrt(CO_base_camera.x*CO_base_camera.x + CO_base_camera.y*CO_base_camera.y));
    
    return !(CO_base_camera.y < 0); // false -> derrière la camera (ne pas afficher)
    // true -> devant la camera (afficher)
}

void rotate(float* x1, float* y1, float angle, SDL_Window* window) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float x1_non_centrer = *x1 - (float)w/2;
    float y1_non_centrer = *y1 - (float)h/2;
    *x1 = x1_non_centrer*cos(angle) + y1_non_centrer*sin(angle) + (float)w/2;
    *y1 = -x1_non_centrer*sin(angle) + y1_non_centrer*cos(angle) + (float)h/2;
}

void draw_triangle(camera_t* camera, triangle_t* triangle, SDL_Color c1, SDL_Color c2, SDL_Color c3, SDL_Window* window, SDL_Renderer* renderer) {
    float theta, phi;
    float x1, y1, x2, y2, x3, y3;

    SDL_bool t1 = get_angles(camera, triangle->p1, &theta, &phi);
    angles_to_screen_coordinates(theta, phi, camera->fov, window, &x1, &y1);
    SDL_bool t2 = get_angles(camera, triangle->p2, &theta, &phi);
    angles_to_screen_coordinates(theta, phi, camera->fov, window, &x2, &y2);
    SDL_bool t3 = get_angles(camera, triangle->p3, &theta, &phi);
    angles_to_screen_coordinates(theta, phi, camera->fov, window, &x3, &y3);

    rotate(&x1, &y1, camera->roll, window);
    rotate(&x2, &y2, camera->roll, window);
    rotate(&x3, &y3, camera->roll, window);

    SDL_Vertex s1 = {{x1, y1}, c1, {1, 1}};
    SDL_Vertex s2 = {{x2, y2}, c2, {1, 1}};
    SDL_Vertex s3 = {{x3, y3}, c3, {1, 1}};

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float px = (float)w/2;
    float py = (float)h/2;
    float areaTriangle = abs_float((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1));
    float area1 = abs_float((x1-px)*(y2-py) - (x2-px)*(y1-py));
    float area2 = abs_float((x2-px)*(y3-py) - (x3-px)*(y2-py));
    float area3 = abs_float((x3-px)*(y1-py) - (x1-px)*(y3-py));
    float sum = area1+area2+area3;

    if (sum > areaTriangle-0.1 && sum < areaTriangle+0.1) {
        camera->triangle_pointer = triangle;
    }

    SDL_Vertex sommets[3] = {s1, s2, s3};

    if (t1 && t2 && t3) SDL_RenderGeometry(renderer, NULL, sommets, 3, NULL, 0);
}

void sort(camera_t* camera, triangle_t** triangles, int size) {
    float* distances = malloc(sizeof(float)*size);
    for (int k = 0; k < size; k++) {
        distances[k] = average_sqared_distance(triangles[k], camera);
    }
    for (int i=1; i<size; i++) {
        int j = i;
        while (j>0 && distances[j] > distances[j-1]) {
            triangle_t* tmp = triangles[j-1];
            triangles[j-1] = triangles[j];
            triangles[j] = tmp;
            float distance_tmp = distances[j-1];
            distances[j-1] = distances[j];
            distances[j] = distance_tmp;
            j--;
        }
    }
    free(distances);
}

float average_sqared_distance(triangle_t* t, camera_t* camera) {
    float dx = t->p1->x - camera->point->position->x;
    float dy = t->p1->y - camera->point->position->y;
    float dz = t->p1->z - camera->point->position->z;
    float dp1 = dx*dx + dy*dy + dz*dz;

    dx = t->p2->x - camera->point->position->x;
    dy = t->p2->y - camera->point->position->y;
    dz = t->p2->z - camera->point->position->z;
    float dp2 = dx*dx + dy*dy + dz*dz;

    dx = t->p3->x - camera->point->position->x;
    dy = t->p3->y - camera->point->position->y;
    dz = t->p3->z - camera->point->position->z;
    float dp3 = dx*dx + dy*dy + dz*dz;
    return (dp1+dp2+dp3)/3;
}

void update_camera(camera_t* camera, list_t* keyDown, float* mouse_events) {
    while(keyDown != NULL) {
        switch (keyDown->val) {
            case SDLK_z :
                camera->point->acceleration->x += camera->keyboard_sensitivity * sin(-camera->yaw) * cos(camera->pitch);
                camera->point->acceleration->y += camera->keyboard_sensitivity * cos(camera->pitch) * cos(camera->yaw);
                camera->point->acceleration->z += camera->keyboard_sensitivity * sin(camera->pitch);
                break;

            case SDLK_q :
                camera->point->acceleration->x += -camera->keyboard_sensitivity * cos(camera->yaw) * cos(camera->roll);
                camera->point->acceleration->y += -camera->keyboard_sensitivity * sin(camera->yaw) * cos(camera->roll);
                camera->point->acceleration->z += -camera->keyboard_sensitivity * sin(-camera->roll); 
                break;

            case SDLK_s :
                camera->point->acceleration->x += -camera->keyboard_sensitivity * sin(-camera->yaw) * cos(camera->pitch);
                camera->point->acceleration->y += -camera->keyboard_sensitivity * cos(camera->pitch) * cos(camera->yaw);
                camera->point->acceleration->z += -camera->keyboard_sensitivity * sin(camera->pitch);
                break;

            case SDLK_d :
                camera->point->acceleration->x += camera->keyboard_sensitivity * cos(camera->yaw) * cos(camera->roll);
                camera->point->acceleration->y += camera->keyboard_sensitivity * sin(camera->yaw) * cos(camera->roll);
                camera->point->acceleration->z += camera->keyboard_sensitivity * sin(-camera->roll); 
                break;

            case SDLK_e :
                camera->roll -= camera->mouse_sensitivity;
                break;

            case SDLK_r :
                camera->roll += camera->mouse_sensitivity;
                break;

            case SDLK_SPACE:
                camera->point->acceleration->z += camera->keyboard_sensitivity;
                break;

            case SDLK_LSHIFT:
                camera->point->acceleration->z -= camera->keyboard_sensitivity;
                break;
            
            case SDLK_LEFT :
                camera->point->position->x -= 0.3;
                break;

            case SDLK_RIGHT :
                camera->point->position->x += 0.3;
                break;

            case SDLK_DOWN :
                camera->point->position->y -= 0.3;
                break;

            case SDLK_UP :
                camera->point->position->y += 0.3;
                break;
            
            case SDLK_PAGEUP :
                camera->point->position->z += 0.3;
                break;

            case SDLK_PAGEDOWN :
                camera->point->position->z -= 0.3;
                break;
        }
        if (keyDown != NULL) keyDown = keyDown->next;
    }

    Uint64 t = SDL_GetTicks64();

    float time_elapsed = (float)(t - camera->point->time)/1000;

    camera->roll -= camera->mouse_sensitivity * mouse_events[0] * 3 * time_elapsed;
    camera->yaw -= camera->mouse_sensitivity * (mouse_events[1] * cos(camera->roll) - mouse_events[2] * sin(camera->roll)) * time_elapsed;
    camera->pitch -= camera->mouse_sensitivity * (mouse_events[1] * sin(camera->roll) + mouse_events[2] * cos(camera->roll)) * time_elapsed;

    float coefficient_frottement = -8;
    copy_point(somme_point(produit_par_scalaire(coefficient_frottement, *camera->point->speed), *camera->point->acceleration), camera->point->acceleration);

    copy_point(somme_point(*camera->point->speed, produit_par_scalaire(time_elapsed, *camera->point->acceleration)), camera->point->speed);
    copy_point(somme_point(*camera->point->position, produit_par_scalaire(time_elapsed, *camera->point->speed)), camera->point->position);
    copy_point(produit_par_scalaire(0, *camera->point->acceleration), camera->point->acceleration);

    camera->point->time = t;
}