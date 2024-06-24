#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include "math_utils.h"
#include "object.h"
#include "physic.h"
#include "list.h"

typedef struct { // 3D object reduce to a point which have for natural pointing toward Oy in Oxyz
    physical_point_t* point;
    float yaw; // rotation à l'horizon pour invariant <--> radian
    float pitch; // rotation à la verticale pour invariant /\ \/ radian
    float roll; // rotation radian
    float keyboard_sensitivity;
    float mouse_sensitivity;
    float fov;
    triangle_t* triangle_pointer;
} camera_t;

camera_t* create_camera (point_t position, float keyboard_sensitivity, float mouse_sensitivity, float fov);
void destroy_camera (camera_t* camera);
void angles_to_screen_coordinates (float theta, float phi, float fov, SDL_Window* window, float* x, float* y);
SDL_bool get_angles (camera_t* C, point_t* O, float* theta, float* phi);
void rotate(float* x1, float* y1, float angle, SDL_Window* window);
void draw_triangle(camera_t* camera, triangle_t* triangle, SDL_Color c1, SDL_Color c2, SDL_Color c3, SDL_Window* window, SDL_Renderer* renderer);
void sort(camera_t* camera, triangle_t** triangles, int size);
float average_sqared_distance(triangle_t* t, camera_t* camera);
void update_camera(camera_t* camera, list_t* keyDown, float* mouse_events);

#endif