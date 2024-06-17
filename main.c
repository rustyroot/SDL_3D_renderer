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
#include <stdbool.h>

#include "camera.h"

int main (void) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    int screen_width = mode.w;
    int screen_height = mode.h;
    float ratio = 0.7;
    float fov = 180;
    float fov_vertical = to_radians(fov*screen_height/screen_width); // 130
    float fov_horizontal = to_radians(fov); // 180

    SDL_CreateWindowAndRenderer((int)(ratio * screen_width), (int)(ratio * screen_height), SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_SetWindowPosition(window, (screen_width - (int)(ratio * screen_width))/2, (screen_height - (int)(ratio * screen_height))/2);

    SDL_bool is_running = SDL_TRUE;
    SDL_Event event;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    float camera_speed = 0.1;
    float camera_sensitivity = to_radians(10.0/60);
    camera_t camera = {0, -10, 0, 0, 0, 0, camera_speed, camera_sensitivity};
    point_t points[12] = {
        {-1, -1, -1}, {1, -1, -1}, {1, -1, 1},
        {-1, -1, -1}, {1, -1, 1}, {-1, -1, 1},
        {1, -1, 1}, {1, -1, -1}, {1, 1, -1},
        {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
    };

    SDL_Color c1 = {255,0,0,255};
    SDL_Color c2 = {0,255,0,255};
    SDL_Color c3 = {0,0,255,255};
    SDL_Color c4 = {255,255,0,255};
    SDL_Color c5 = {255,0,255,255};
    SDL_Color c6 = {0,255,255,255};

    int nbt;
    triangle_t* tt = load_obj_file("suzanne.obj", &nbt);

    int nombre_case_damier = 51;
    int nombre_triangle = 4+nombre_case_damier*nombre_case_damier*2 + nbt;
    triangle_t** triangles = malloc(sizeof(triangle_t*)*nombre_triangle);
    for (int i=0; i<4; i++) triangles[i] = init_triangle(points[i*3], points[i*3+1], points[i*3+2], (i<2)?c1:c2);
    int si = 1.5;
    int px = -nombre_case_damier*si/2;
    int py = -nombre_case_damier*si/2;
    int pz = -2;
    for (int i=0; i<nombre_case_damier*nombre_case_damier; i++) {
        int x = i/nombre_case_damier;
        int y = i%nombre_case_damier;
        point_t p1 = {px + x*si, py + y*si, pz};
        point_t p2 = {px + (x+1)*si, py + (y+1)*si, pz};
        point_t p3 = {px + (x+1)*si, py + y*si, pz};
        triangles[4+i*2] = init_triangle(p1, p2, p3, (i%2 == 0)?c5:c6);
        p3.x = px + x*si;
        p3.y = py + (y+1)*si;
        triangles[4+i*2+1] = init_triangle(p1, p2, p3, (i%2 == 0)?c5:c6);
    }

    point_t decalage = {3,0,0};
    point_t r1 = {1, 0, 0};
    point_t r2 = {0, 0, -1};
    point_t r3 = {0, 1, 0};
    for (int i=0; i<nbt; i++) {
        point_t p1 = {
            r1.x*tt[i].p1.x + r1.y*tt[i].p1.y + r1.z*tt[i].p1.z + decalage.x,
            r2.x*tt[i].p1.x + r2.y*tt[i].p1.y + r2.z*tt[i].p1.z + decalage.y,
            r3.x*tt[i].p1.x + r3.y*tt[i].p1.y + r3.z*tt[i].p1.z + decalage.z,
        };
        point_t p2 = {
            r1.x*tt[i].p2.x + r1.y*tt[i].p2.y + r1.z*tt[i].p2.z + decalage.x,
            r2.x*tt[i].p2.x + r2.y*tt[i].p2.y + r2.z*tt[i].p2.z + decalage.y,
            r3.x*tt[i].p2.x + r3.y*tt[i].p2.y + r3.z*tt[i].p2.z + decalage.z,
        };
        point_t p3 = {
            r1.x*tt[i].p3.x + r1.y*tt[i].p3.y + r1.z*tt[i].p3.z + decalage.x,
            r2.x*tt[i].p3.x + r2.y*tt[i].p3.y + r2.z*tt[i].p3.z + decalage.y,
            r3.x*tt[i].p3.x + r3.y*tt[i].p3.y + r3.z*tt[i].p3.z + decalage.z,
        };

        triangle_t* tr = malloc(sizeof(triangle_t));
        tr->p1 = p1;
        tr->p2 = p2;
        tr->p3 = p3;
        tr->color = tt[i].color;

        triangles[nombre_triangle-nbt+i] = tr;
    }

    while (is_running) {

        // printf("time  = %ld ms\n", SDL_GetTicks64());
        // printf("position camera : x:%f, y:%f, z:%f, yaw:%f, pitch:%f, roll:%f\n", camera.x, camera.y, camera.z, camera.yaw, camera.pitch, camera.roll);

        while(SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_KEYDOWN :
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE :
                            is_running = SDL_FALSE;
                            break;

                        case SDLK_z :
                            camera.x += camera.speed * sin(-camera.yaw) * cos(camera.pitch);
                            camera.y += camera.speed * cos(camera.pitch) * cos(camera.yaw);
                            camera.z += camera.speed * sin(camera.pitch);
                            break;

                        case SDLK_q :
                            camera.x -= camera.speed * cos(camera.yaw) * cos(camera.roll);
                            camera.y -= camera.speed * sin(camera.yaw) * cos(camera.roll);
                            camera.z -= camera.speed * sin(-camera.roll); 
                            break;

                        case SDLK_s :
                            camera.x -= camera.speed * sin(-camera.yaw) * cos(camera.pitch);
                            camera.y -= camera.speed * cos(camera.pitch) * cos(camera.yaw);
                            camera.z -= camera.speed * sin(camera.pitch);
                            break;

                        case SDLK_d :
                            camera.x += camera.speed * cos(camera.yaw) * cos(camera.roll);
                            camera.y += camera.speed * sin(camera.yaw) * cos(camera.roll);
                            camera.z += camera.speed * sin(-camera.roll); 
                            break;

                        case SDLK_e :
                            camera.roll -= camera_sensitivity;
                            break;

                        case SDLK_r :
                            camera.roll += camera_sensitivity;
                            break;
                        
                        case SDLK_LEFT :
                            camera.x -= camera.speed;
                            break;

                        case SDLK_RIGHT :
                            camera.x += camera.speed;
                            break;

                        case SDLK_DOWN :
                            camera.y -= camera.speed;
                            break;

                        case SDLK_UP :
                            camera.y += camera.speed;
                            break;
                        
                        case SDLK_PAGEUP :
                            camera.z += camera.speed;
                            break;

                        case SDLK_PAGEDOWN :
                            camera.z -= camera.speed;
                            break;
                    }
                    break;

                case SDL_MOUSEWHEEL : 
                    camera.roll -= camera_sensitivity * event.wheel.preciseY * 3;
                    break;

                case SDL_MOUSEMOTION :
                    camera.yaw -= camera.sensitivity * (event.motion.xrel * cos(camera.roll) - event.motion.yrel * sin(camera.roll));
                    camera.pitch -= camera.sensitivity * (event.motion.xrel * sin(camera.roll) + event.motion.yrel * cos(camera.roll));
                    break;

                case SDL_QUIT:
                    is_running = SDL_FALSE;
                    break;
            }
            
        }
        SDL_RenderClear(renderer);

        sort(&camera, triangles, nombre_triangle);
        for (int i=0; i<nombre_triangle; i++) {
            draw_triangle(&camera, &(triangles[i]->p1), &(triangles[i]->p2), &(triangles[i]->p3), triangles[i]->color, triangles[i]->color, triangles[i]->color, fov_horizontal, fov_vertical, window, renderer);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderPresent(renderer);

    }

    for (int i=0; i<nombre_triangle; i++) free(triangles[i]);
    free(triangles);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
