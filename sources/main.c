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

#include "../includes/camera.h"
#include "../includes/math_utils.h"
#include "../includes/objet.h"

int main (void) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    int screen_width = mode.w;
    int screen_height = mode.h;
    float ratio = 0.7;
    float fov = 90;
    float fov_vertical = to_radians(fov*screen_height/screen_width); // 130
    float fov_horizontal = to_radians(fov); // 180

    SDL_CreateWindowAndRenderer((int)(ratio * screen_width), (int)(ratio * screen_height), SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_SetWindowPosition(window, (screen_width - (int)(ratio * screen_width))/2, (screen_height - (int)(ratio * screen_height))/2);

    SDL_bool is_running = SDL_TRUE;
    SDL_Event event;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    float camera_keyboard_sensitivity = 0.1;
    float camera_sensitivity = to_radians(10.0/60);
    camera_t camera = {0, -10, 0, 0, 0, 0, {0,0,0}, {0,0,0}, camera_keyboard_sensitivity, camera_sensitivity};
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

    int nbt;
    triangle_t* tt = load_obj_file("objects/suzanne.obj", &nbt);

    int nombre_case_damier = 51;
    int nombre_triangle = 4+nombre_case_damier*nombre_case_damier*2 + nbt;
    triangle_t** triangles = malloc(sizeof(triangle_t*)*nombre_triangle);
    for (int i=0; i<4; i++) triangles[i] = init_triangle(points[i*3], points[i*3+1], points[i*3+2], (i<2)?red:green);
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
        triangles[4+i*2] = init_triangle(p1, p2, p3, (i%2 == 0)?magenta:cyan);
        p3.x = px + x*si;
        p3.y = py + (y+1)*si;
        triangles[4+i*2+1] = init_triangle(p1, p2, p3, (i%2 == 0)?magenta:cyan);
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
        
        while(SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_KEYDOWN :
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE :
                            is_running = SDL_FALSE;
                            break;

                        case SDLK_z :
                            camera.acceleration.x = camera.keyboard_sensitivity * sin(-camera.yaw) * cos(camera.pitch);
                            camera.acceleration.y = camera.keyboard_sensitivity * cos(camera.pitch) * cos(camera.yaw);
                            camera.acceleration.z = camera.keyboard_sensitivity * sin(camera.pitch);
                            break;

                        case SDLK_q :
                            camera.acceleration.x = -camera.keyboard_sensitivity * cos(camera.yaw) * cos(camera.roll);
                            camera.acceleration.y = -camera.keyboard_sensitivity * sin(camera.yaw) * cos(camera.roll);
                            camera.acceleration.z = -camera.keyboard_sensitivity * sin(-camera.roll); 
                            break;

                        case SDLK_s :
                            camera.acceleration.x = -camera.keyboard_sensitivity * sin(-camera.yaw) * cos(camera.pitch);
                            camera.acceleration.y = -camera.keyboard_sensitivity * cos(camera.pitch) * cos(camera.yaw);
                            camera.acceleration.z = -camera.keyboard_sensitivity * sin(camera.pitch);
                            break;

                        case SDLK_d :
                            camera.acceleration.x = camera.keyboard_sensitivity * cos(camera.yaw) * cos(camera.roll);
                            camera.acceleration.y = camera.keyboard_sensitivity * sin(camera.yaw) * cos(camera.roll);
                            camera.acceleration.z = camera.keyboard_sensitivity * sin(-camera.roll); 
                            break;

                        case SDLK_e :
                            camera.roll -= camera.sensitivity;
                            break;

                        case SDLK_r :
                            camera.roll += camera.sensitivity;
                            break;
                        
                        // case SDLK_LEFT :
                        //     camera.x -= camera.speed;
                        //     break;

                        // case SDLK_RIGHT :
                        //     camera.x += camera.speed;
                        //     break;

                        // case SDLK_DOWN :
                        //     camera.y -= camera.speed;
                        //     break;

                        // case SDLK_UP :
                        //     camera.y += camera.speed;
                        //     break;
                        
                        // case SDLK_PAGEUP :
                        //     camera.z += camera.speed;
                        //     break;

                        // case SDLK_PAGEDOWN :
                        //     camera.z -= camera.speed;
                        //     break;
                    }
                    break;

                case SDL_MOUSEWHEEL : 
                    camera.roll -= camera.sensitivity * event.wheel.preciseY * 3;
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

        camera.speed.x += camera.acceleration.x;
        camera.speed.y += camera.acceleration.y;
        camera.speed.z += camera.acceleration.z;
        camera.speed.x *= 0.9;
        camera.speed.y *= 0.9;
        camera.speed.z *= 0.9;
        camera.x += camera.speed.x;
        camera.y += camera.speed.y;
        camera.z += camera.speed.z;

        camera.acceleration.x = 0;
        camera.acceleration.y = 0;
        camera.acceleration.z = 0;
        

        SDL_RenderClear(renderer);

        sort(&camera, triangles, nombre_triangle);
        for (int i=0; i<nombre_triangle; i++) {
            draw_triangle(&camera, &(triangles[i]->p1), &(triangles[i]->p2), &(triangles[i]->p3), triangles[i]->color, triangles[i]->color, triangles[i]->color, fov_horizontal, fov_vertical, window, renderer);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
        SDL_RenderPresent(renderer);
        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(Message);

    }

    free(time_elapsed);
    for (int i=0; i<nombre_triangle; i++) free(triangles[i]);
    free(triangles);

    TTF_CloseFont(liberation);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
