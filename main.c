#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>

typedef struct { // 3D object reduce to a point which have for natural pointing toward Oy in Oxyz
    float x;
    float y;
    float z;
    float yaw; // rotation à l'horizon pour invariant <--> radian
    float pitch; // rotation à la verticale pour invariant /\ \/ radian
    float roll; // rotation radian
    float speed;
    float sensitivity;
} camera_t;

typedef struct {
    float x;
    float y;
    float z;
} point_t;

double to_degrees(double radians) {
    return radians * (180.0 / M_PI);
}

double to_radians(double degrees) {
    return degrees * (M_PI / 180.0);
}

void angles_to_screen_coordinates (float theta, float phi, float fov_horizontal, float fov_vertical, SDL_Window* window, float* x, float* y) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    *x = (2 * theta / fov_horizontal) * w + 0.5*w;
    *y = (2 * phi / fov_vertical) * h + 0.5*h;
}

void get_angles (camera_t* C, point_t* O, float* theta, float* phi) {
    point_t CP = {sin(C->yaw) * cos(C->pitch), cos(C->pitch) * cos(C->yaw), sin(C->pitch)};
    point_t CO = {O->x - C->x, O->y - C->y, O->z - C->z};

    float CPxy_dot_COxy = CP.x * CO.x + CP.y * CO.y;
    float COxy_norm = sqrtf(CO.x * CO.x + CO.y * CO.y);
    float CPxy_norm = sqrtf(CP.x * CP.x + CP.y * CP.y);

    point_t CP_90_yaw = {sin(C->yaw+M_PI/2) * cos(C->pitch), cos(C->pitch) * cos(C->yaw+M_PI/2), sin(C->pitch)};
    float CP_90_yawxy_dot_COxy = CP_90_yaw.x * CO.x + CP_90_yaw.y * CO.y;

    *theta = acos(CPxy_dot_COxy / (COxy_norm * CPxy_norm)) * ((CP_90_yawxy_dot_COxy>0)?1:-1);

    float CPyz_dot_COyz = CP.y * CO.y + CP.z * CO.z;
    float CPyz_norm = sqrtf(CP.y * CP.y + CP.z * CP.z);
    float COyz_norm = sqrtf(CO.y * CO.y + CO.z * CO.z);

    point_t CP_90_pitch = {sin(C->yaw) * cos(C->pitch+M_PI/2), cos(C->pitch+M_PI/2) * cos(C->yaw), sin(C->pitch+M_PI/2)};
    float CP_90_pitchxy_dot_COxy = CP_90_pitch.y * CO.y + CP_90_pitch.z * CO.z;

    *phi = acos(CPyz_dot_COyz / (COyz_norm * CPyz_norm)) * ((CP_90_pitchxy_dot_COxy>0)?1:-1);
}

int main (void) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    int screen_width = mode.w;
    int screen_height = mode.h;
    float ratio = 0.5;
    float fov_vertical = to_radians(130);
    float fov_horizontal = to_radians(180);

    SDL_CreateWindowAndRenderer((int)(ratio * screen_width), (int)(ratio * screen_height), SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_SetWindowPosition(window, (screen_width - (int)(ratio * screen_width))/2, (screen_height - (int)(ratio * screen_height))/2);

    SDL_bool is_running = SDL_TRUE;
    SDL_Event event;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    float camera_speed = 0.05;
    float camera_sensitivity = to_radians(10.0/60);
    camera_t camera = {0, 0, 0, 0, 0, 0, camera_speed, camera_sensitivity};
    point_t point1 = {0, 3, 0};
    point_t point2 = {3, 3, 0};
    point_t point3 = { 3, 3, 3};

    while (is_running) {

        printf("time  = %ld ms\n", SDL_GetTicks64());

        while(SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_KEYDOWN :
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE :
                            is_running = SDL_FALSE;
                            break;

                        case SDLK_z :
                            camera.x += camera.speed * sin(camera.yaw) * cos(camera.pitch);
                            camera.y += camera.speed * cos(camera.pitch) * cos(camera.yaw);
                            camera.z += camera.speed * sin(camera.pitch);
                            break;

                        case SDLK_q :
                            camera.x -= camera.speed * cos(camera.yaw) * cos(camera.roll);
                            camera.y -= camera.speed * sin(-camera.yaw) * cos(camera.roll);
                            camera.z -= camera.speed * sin(-camera.roll); 
                            break;

                        case SDLK_s :
                            camera.x -= camera.speed * sin(camera.yaw) * cos(camera.pitch);
                            camera.y -= camera.speed * cos(camera.pitch) * cos(camera.yaw);
                            camera.z -= camera.speed * sin(camera.pitch);
                            break;

                        case SDLK_d :
                            camera.x += camera.speed * cos(camera.yaw) * cos(camera.roll);
                            camera.y += camera.speed * sin(-camera.yaw) * cos(camera.roll);
                            camera.z += camera.speed * sin(-camera.roll); 
                            break;

                        case SDLK_e :
                            camera.roll -= camera_sensitivity;
                            break;

                        case SDLK_r :
                            camera.roll += camera_sensitivity;
                            break;
                    }
                    //printf("%f\n", sqrtf(sin(camera.yaw) * cos(camera.pitch) * sin(camera.yaw) * cos(camera.pitch) + cos(camera.pitch) * cos(camera.yaw) * cos(camera.pitch) * cos(camera.yaw) + sin(camera.pitch) * sin(camera.pitch)));
                    break;

                case SDL_MOUSEWHEEL : 
                    camera.yaw -= event.wheel.preciseX * camera.sensitivity;
                    camera.pitch -= event.wheel.preciseY * camera.sensitivity;
                    break;

                case SDL_MOUSEMOTION :
                    camera.yaw += event.motion.xrel * camera.sensitivity;
                    camera.pitch += event.motion.yrel * camera.sensitivity;
                    break;

                case SDL_QUIT:
                    is_running = SDL_FALSE;
                    break;
            }
            
        }

        float theta, phi;
        float x1, y1, x2, y2, x3, y3;

        

        get_angles(&camera, &point1, &theta, &phi);
        angles_to_screen_coordinates(theta, phi, fov_horizontal, fov_vertical, window, &x1, &y1);
        get_angles(&camera, &point2, &theta, &phi);
        angles_to_screen_coordinates(theta, phi, fov_horizontal, fov_vertical, window, &x2, &y2);
        get_angles(&camera, &point3, &theta, &phi);
        angles_to_screen_coordinates(theta, phi, fov_horizontal, fov_vertical, window, &x3, &y3);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Vertex s1 = {{x1, y1}, {255, 0, 0, 255}, {1, 1}};
        SDL_Vertex s2 = {{x2, y2}, {0, 255, 0, 255}, {1, 1}};
        SDL_Vertex s3 = {{x3, y3}, {0, 0, 255, 255}, {1, 1}};
        SDL_Vertex sommets[3] = {s1, s2, s3};
        SDL_RenderGeometry(renderer, NULL, sommets, 3, NULL, 0);
        SDL_RenderPresent(renderer);

    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}