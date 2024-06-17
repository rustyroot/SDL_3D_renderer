#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include "math_utils.h"

typedef struct { // 3D object reduce to a point which have for natural pointing toward Oy in Oxyz
    float x;
    float y;
    float z;
    float yaw; // rotation à l'horizon pour invariant <--> radian
    float pitch; // rotation à la verticale pour invariant /\ \/ radian
    float roll; // rotation radian
    point_t speed;
    point_t acceleration;
    float keyboard_sensitivity;
    float sensitivity;
} camera_t;

void angles_to_screen_coordinates (float theta, float phi, float fov_horizontal, float fov_vertical, SDL_Window* window, float* x, float* y);
SDL_bool get_angles (camera_t* C, point_t* O, float* theta, float* phi);
void rotate(float* x1, float* y1, float angle, SDL_Window* window);
void draw_triangle(camera_t* camera, point_t* point1, point_t* point2, point_t* point3, SDL_Color c1, SDL_Color c2, SDL_Color c3, float fov_horizontal, float fov_vertical, SDL_Window* window, SDL_Renderer* renderer);
void sort(camera_t* camera, triangle_t** triangles, int size);
float distance(triangle_t* t, camera_t* camera);

#endif