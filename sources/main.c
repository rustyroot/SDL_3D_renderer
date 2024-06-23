#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>

#include "../includes/camera.h"
#include "../includes/math_utils.h"
#include "../includes/object.h"
#include "../includes/list.h"
#include "../includes/sdl_utils.h"

int main (void) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    int screen_width = mode.w;
    int screen_height = mode.h;
    float ratio = 0.7;
    float fov = to_radians(60);

    SDL_CreateWindowAndRenderer((int)(ratio * screen_width), (int)(ratio * screen_height), SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_SetWindowPosition(window, (screen_width - (int)(ratio * screen_width))/2, (screen_height - (int)(ratio * screen_height))/2);

    SDL_bool is_running = SDL_TRUE;
    SDL_Event event;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    float camera_keyboard_sensitivity = 100;
    float camera_mouse_sensitivity = to_radians(10.0/60);
    camera_t* camera = create_camera((point_t){0, -10, 0}, camera_keyboard_sensitivity, camera_mouse_sensitivity, fov);

    point_t points[12] = {
        {-1, -1, -1}, {1, -1, -1}, {1, -1, 1},
        {-1, -1, -1}, {1, -1, 1}, {-1, -1, 1},
        {1, -1, 1}, {1, -1, -1}, {1, 1, -1},
        {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
    };

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255,0,0,255};
    SDL_Color green = {0,255,0,255};
    // SDL_Color blue = {0,0,255,255};
    // SDL_Color Jaune = {255,255,0,255};
    SDL_Color magenta = {255,0,255,255};
    SDL_Color cyan = {0,255,255,255};

    int nb_obj = 3;

    objet_t** objects = (objet_t**) malloc(sizeof(objet_t*)*nb_obj);

    objects[0] = (objet_t*) malloc(sizeof(objet_t));
    objects[0]->size = 4;
    objects[0]->triangles = malloc(sizeof(triangle_t*)*objects[0]->size);
    for (int i=0; i<4; i++) objects[0]->triangles[i] = init_triangle(points[i*3], points[i*3+1], points[i*3+2], (i<2)?red:green);

    int nombre_case_damier = 21;
    objects[1] = (objet_t*) malloc(sizeof(objet_t));
    objects[1]->size = nombre_case_damier*nombre_case_damier*2;
    objects[1]->triangles = malloc(sizeof(triangle_t*)*objects[1]->size);
    float si = 1.5;
    int px = -nombre_case_damier*si/2;
    int py = -nombre_case_damier*si/2;
    int pz = -2;
    for (int i=0; i<nombre_case_damier*nombre_case_damier; i++) {
        int x = i/nombre_case_damier;
        int y = i%nombre_case_damier;
        point_t p1 = {px + x*si, py + y*si, pz};
        point_t p2 = {px + (x+1)*si, py + (y+1)*si, pz};
        point_t p3 = {px + (x+1)*si, py + y*si, pz};
        objects[1]->triangles[i*2] = init_triangle(p1, p2, p3, (i%2 == 0)?magenta:cyan);
        p3.x = px + x*si;
        p3.y = py + (y+1)*si;
        objects[1]->triangles[i*2+1] = init_triangle(p1, p2, p3, (i%2 == 0)?magenta:cyan);
    }


    // dragon_model.obj 26000 faces
    // dragon_model2.obj 2000 faces
    // objects[2] = load_obj_file("objects/dragon_model.obj");
    objects[2] = load_obj_file("objects/suzanne.obj");

    point_t decalage = {3,0,0};
    point_t r1 = {3, 0, 0};
    point_t r2 = {0, 0, -3};
    point_t r3 = {0, 3, 0};
    for (int i=0; i<objects[2]->size; i++) {
        point_t p1 = {
            r1.x*objects[2]->triangles[i]->p1.x + r1.y*objects[2]->triangles[i]->p1.y + r1.z*objects[2]->triangles[i]->p1.z + decalage.x,
            r2.x*objects[2]->triangles[i]->p1.x + r2.y*objects[2]->triangles[i]->p1.y + r2.z*objects[2]->triangles[i]->p1.z + decalage.y,
            r3.x*objects[2]->triangles[i]->p1.x + r3.y*objects[2]->triangles[i]->p1.y + r3.z*objects[2]->triangles[i]->p1.z + decalage.z,
        };
        point_t p2 = {
            r1.x*objects[2]->triangles[i]->p2.x + r1.y*objects[2]->triangles[i]->p2.y + r1.z*objects[2]->triangles[i]->p2.z + decalage.x,
            r2.x*objects[2]->triangles[i]->p2.x + r2.y*objects[2]->triangles[i]->p2.y + r2.z*objects[2]->triangles[i]->p2.z + decalage.y,
            r3.x*objects[2]->triangles[i]->p2.x + r3.y*objects[2]->triangles[i]->p2.y + r3.z*objects[2]->triangles[i]->p2.z + decalage.z,
        };
        point_t p3 = {
            r1.x*objects[2]->triangles[i]->p3.x + r1.y*objects[2]->triangles[i]->p3.y + r1.z*objects[2]->triangles[i]->p3.z + decalage.x,
            r2.x*objects[2]->triangles[i]->p3.x + r2.y*objects[2]->triangles[i]->p3.y + r2.z*objects[2]->triangles[i]->p3.z + decalage.y,
            r3.x*objects[2]->triangles[i]->p3.x + r3.y*objects[2]->triangles[i]->p3.y + r3.z*objects[2]->triangles[i]->p3.z + decalage.z,
        };

        objects[2]->triangles[i]->p1 = p1;
        objects[2]->triangles[i]->p2 = p2;
        objects[2]->triangles[i]->p3 = p3;
    }

    int nombre_triangle = 0;
    for (int i=0; i<nb_obj; i++) nombre_triangle += objects[i]->size;
    triangle_t** triangles = malloc(sizeof(triangle_t*)*nombre_triangle);
    int k = 0;
    for (int i=0; i<nb_obj; i++) {
        for (int j=0; j<objects[i]->size; j++) {
            triangles[k] = objects[i]->triangles[j];
            k++;
        }
    }

    TTF_Init();
    TTF_Font* liberation = TTF_OpenFont("fonts/liberation-fonts-ttf-2.1.5/LiberationSerif-Regular.ttf", 24);
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0;  //controls the rect's x coordinate 
    Message_rect.y = 0; // controls the rect's y coordinte

    char* timetext = malloc(sizeof(char) * 100);
    int time_prev, time_next;
    float average_time_elapsed = 4;
    int nb_frames_average = 30;
    int* time_elapsed = calloc(nb_frames_average, sizeof(int));
    for (int i=0; i<nb_frames_average; i++) time_elapsed[i] = 4;
    int index_frame = 0;
    time_prev = SDL_GetTicks64();

    list_t* keyDown = NULL;

    while (is_running) {

        time_next = SDL_GetTicks64();
        int new_time_elapsed = (int) time_next - time_prev;
        average_time_elapsed = (average_time_elapsed * nb_frames_average + new_time_elapsed - time_elapsed[index_frame])/nb_frames_average;
        time_elapsed[index_frame] = new_time_elapsed;
        index_frame = (index_frame+1)%nb_frames_average;
        time_prev = time_next;
        
        sprintf(timetext, "Average_time_elapsed  = %f fps\n", 1000/average_time_elapsed);
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(liberation, timetext, white);
        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
        TTF_SizeText(liberation, timetext, &Message_rect.w, &Message_rect.h);

        float mouse_events[3] = {0, 0, 0};
        poll_event(&event, &keyDown, &is_running, mouse_events);
        update_camera(camera, keyDown, mouse_events);

        SDL_RenderClear(renderer);

        float periode = 5; // s
        float amplitude = 0.03; // space unit
        // mouvement de la forme A*cos(wt) + x0
        // donc vitesse de la forme -A*w*sin(wt)
        float omega = 2 * M_PI * 1/periode;
        float speed_amplitude = -amplitude*omega;
        float t = (float)time_next/1000; // temps courant en seconde
        point_t suzanne_speed = {speed_amplitude * SDL_sinf(omega * t), 0, 0};
        move_obj(objects[2], &suzanne_speed);

        sort(camera, triangles, nombre_triangle);
        for (int i=0; i<nombre_triangle; i++) {
            draw_triangle(camera, &(triangles[i]->p1), &(triangles[i]->p2), &(triangles[i]->p3), triangles[i]->color, triangles[i]->color, triangles[i]->color, window, renderer);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
        SDL_RenderPresent(renderer);
        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(Message);

    }

    free(timetext);

    free(time_elapsed);
    for (int i=0; i<nombre_triangle; i++) free(triangles[i]);
    free(triangles);
    for (int i=0; i<nb_obj; i++) {
        free(objects[i]->triangles);
        free(objects[i]);
    }
    free(objects);

    destroy_camera(camera);
    TTF_CloseFont(liberation);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
