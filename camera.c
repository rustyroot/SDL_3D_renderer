#include "camera.h"

void angles_to_screen_coordinates (float theta, float phi, float fov_horizontal, float fov_vertical, SDL_Window* window, float* x, float* y) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    *x = (2 * theta / fov_horizontal) * w + 0.5*w;
    *y = -(2 * phi / fov_vertical) * h + 0.5*h;
}

bool get_angles (camera_t* C, point_t* O, float* theta, float* phi) {
    point_t CP_1 = {cos(C->yaw), sin(C->yaw), 0};
    point_t CP_2 = {sin(-C->yaw) * cos(C->pitch), cos(C->pitch) * cos(C->yaw), sin(C->pitch)};
    point_t CP_3 = produit_vectoriel(CP_1, CP_2);
    
    point_t CO = {O->x - C->x, O->y - C->y, O->z - C->z};

    point_t CO_base_camera = {
        CP_1.x*CO.x + CP_1.y*CO.y + CP_1.z*CO.z,
        CP_2.x*CO.x + CP_2.y*CO.y + CP_2.z*CO.z,
        CP_3.x*CO.x + CP_3.y*CO.y + CP_3.z*CO.z,
    };

    *phi = asin(CO_base_camera.z / sqrt(CO_base_camera.y*CO_base_camera.y + CO_base_camera.z*CO_base_camera.z));
    *theta = asin(CO_base_camera.x / sqrt(CO_base_camera.x*CO_base_camera.x + CO_base_camera.y*CO_base_camera.y));
    
    if (CO_base_camera.y < 0) return false; // derrière la camera (ne pas afficher)
    else return true;  // devant la camera (afficher)
}

void rotate(float* x1, float* y1, float angle, SDL_Window* window) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float x1_non_centrer = *x1 - w/2;
    float y1_non_centrer = *y1 - h/2;
    *x1 = x1_non_centrer*cos(angle) + y1_non_centrer*sin(angle) + w/2;
    *y1 = -x1_non_centrer*sin(angle) + y1_non_centrer*cos(angle) + h/2;
}

void draw_triangle(camera_t* camera, point_t* point1, point_t* point2, point_t* point3, SDL_Color c1, SDL_Color c2, SDL_Color c3, float fov_horizontal, float fov_vertical, SDL_Window* window, SDL_Renderer* renderer) {
    float theta, phi;
    float x1, y1, x2, y2, x3, y3;

    bool t1 = get_angles(camera, point1, &theta, &phi);
    angles_to_screen_coordinates(theta, phi, fov_horizontal, fov_vertical, window, &x1, &y1);
    bool t2 = get_angles(camera, point2, &theta, &phi);
    angles_to_screen_coordinates(theta, phi, fov_horizontal, fov_vertical, window, &x2, &y2);
    bool t3 = get_angles(camera, point3, &theta, &phi);
    angles_to_screen_coordinates(theta, phi, fov_horizontal, fov_vertical, window, &x3, &y3);

    rotate(&x1, &y1, camera->roll, window);
    rotate(&x2, &y2, camera->roll, window);
    rotate(&x3, &y3, camera->roll, window);

    SDL_Vertex s1 = {{x1, y1}, c1, {1, 1}};
    SDL_Vertex s2 = {{x2, y2}, c2, {1, 1}};
    SDL_Vertex s3 = {{x3, y3}, c3, {1, 1}};
    SDL_Vertex sommets[3] = {s1, s2, s3};

    if (t1 && t2 && t3) SDL_RenderGeometry(renderer, NULL, sommets, 3, NULL, 0);
}

void sort(camera_t* camera, triangle_t** triangles, int size) {
    for (int i=1; i<size; i++) {
        int j = i;
        while (j>0 && distance(triangles[j], camera) > distance(triangles[j-1], camera)) {
            triangle_t* tmp = triangles[j-1];
            triangles[j-1] = triangles[j];
            triangles[j] = tmp;
            j--;
        }
    }
}