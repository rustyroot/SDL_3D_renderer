#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "camera.h"
#include "math_utils.h"
#include "object.h"
#include "list.h"
#include "sdl_utils.h"

const char* help_message = 
    "\
    g -> activer / désactiver la physique\n\
    p -> ajouter un cube\n\
    z -> avancer\n\
    s -> reculer\n\
    q -> aller à gauche\n\
    d -> aller à droite\n\
    space -> monter\n\
    left shift -> descendre\n\
    e -> rotation de la caméra vers la gauche\n\
    r -> rotation de la caméra vers la droite\n\
    h -> activer / désactiver le message d'aide\n\
    escape -> quiter\
    ";

int main (void) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    int screen_width = mode.w;
    int screen_height = mode.h;
    float ratio = 1;
    float fov = to_radians(60);

    SDL_CreateWindowAndRenderer((int)(ratio * screen_width), (int)(ratio * screen_height), SDL_WINDOW_SHOWN, &window, &renderer);
    //SDL_SetWindowPosition(window, (screen_width - (int)(ratio * screen_width))/2, (screen_height - (int)(ratio * screen_height))/2);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    SDL_bool is_running = SDL_TRUE;
    SDL_Event event;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    float camera_keyboard_sensitivity = 100;
    float camera_mouse_sensitivity = to_radians(15);
    camera_t* camera = create_camera(malloc_point((point_t){0, -10, 0}), camera_keyboard_sensitivity, camera_mouse_sensitivity, fov);

    // point_t points[12] = {
    //     {-1, -1, -1}, {1, -1, -1}, {1, -1, 1},
    //     {-1, -1, -1}, {1, -1, 1}, {-1, -1, 1},
    //     {1, -1, 1}, {1, -1, -1}, {1, 1, -1},
    //     {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
    // };

    SDL_Color white = {255, 255, 255, 255};
    // SDL_Color red = {255,0,0,255};
    // SDL_Color green = {0,255,0,255};
    // SDL_Color blue = {0,0,255,255};
    // SDL_Color Jaune = {255,255,0,255};
    SDL_Color magenta = {255,0,255,255};
    SDL_Color cyan = {0,255,255,255};

    int nb_obj = 2;

    objet_t** objects = (objet_t**) malloc(sizeof(objet_t*)*nb_obj);

    // objects[0] = (objet_t*) malloc(sizeof(objet_t));
    // objects[0]->nb_triangles = 4;
    // objects[0]->triangles = malloc(sizeof(triangle_t*)*objects[0]->nb_triangles);
    // for (int i=0; i<objects[0]->nb_triangles; i++) objects[0]->triangles[i] = init_triangle(points[i*3], points[i*3+1], points[i*3+2], (i<2)?red:green);
    // objects[0]->nb_springs = 0;
    // objects[0]->springs = NULL;

    int nombre_case_damier = 21;
    objects[0] = (objet_t*) malloc(sizeof(objet_t));
    objects[0]->nb_sommets = (nombre_case_damier+1)*(nombre_case_damier+1);
    objects[0]->nb_triangles = nombre_case_damier*nombre_case_damier*2;
    objects[0]->sommets = malloc(sizeof(point_t*)*objects[0]->nb_sommets);
    objects[0]->triangles_indice = malloc(sizeof(int*)*objects[0]->nb_triangles);
    objects[0]->triangles = malloc(sizeof(triangle_t*)*objects[0]->nb_triangles);
    float si = 1.5;
    int px = -nombre_case_damier*si/2;
    int py = -nombre_case_damier*si/2;
    int pz = -2;
    for (int i=0; i<nombre_case_damier+1; i++) {
        for (int j=0; j<nombre_case_damier+1; j++) {
            objects[0]->sommets[i*(nombre_case_damier+1)+j] = create_physical_point(malloc_point((point_t){px + i*si, py+j*si, pz}), 0, 1);
        }
    }
    for (int i=0; i<nombre_case_damier*nombre_case_damier; i++) {
        int x = i/nombre_case_damier;
        int y = i%nombre_case_damier;

        objects[0]->triangles_indice[i*2] = malloc(sizeof(int)*3);
        objects[0]->triangles_indice[i*2][0] = x*(nombre_case_damier+1)+y;
        objects[0]->triangles_indice[i*2][1] = x*(nombre_case_damier+1)+(y+1);
        objects[0]->triangles_indice[i*2][2] = (x+1)*(nombre_case_damier+1)+y;
        objects[0]->triangles_indice[i*2+1] = malloc(sizeof(int)*3);
        objects[0]->triangles_indice[i*2+1][0] = x*(nombre_case_damier+1)+(y+1);
        objects[0]->triangles_indice[i*2+1][1] = (x+1)*(nombre_case_damier+1)+y;
        objects[0]->triangles_indice[i*2+1][2] = (x+1)*(nombre_case_damier+1)+(y+1);

        objects[0]->triangles[i*2] = malloc(sizeof(triangle_t));
        objects[0]->triangles[i*2]->color = (i%2 == 0)?magenta:cyan;
        objects[0]->triangles[i*2]->p1 = objects[0]->sommets[objects[0]->triangles_indice[i*2][0]]->position;
        objects[0]->triangles[i*2]->p2 = objects[0]->sommets[objects[0]->triangles_indice[i*2][1]]->position;
        objects[0]->triangles[i*2]->p3 = objects[0]->sommets[objects[0]->triangles_indice[i*2][2]]->position;

        objects[0]->triangles[i*2+1] = malloc(sizeof(triangle_t));
        objects[0]->triangles[i*2+1]->color = (i%2 == 0)?magenta:cyan;
        objects[0]->triangles[i*2+1]->p1 = objects[0]->sommets[objects[0]->triangles_indice[i*2+1][0]]->position;
        objects[0]->triangles[i*2+1]->p2 = objects[0]->sommets[objects[0]->triangles_indice[i*2+1][1]]->position;
        objects[0]->triangles[i*2+1]->p3 = objects[0]->sommets[objects[0]->triangles_indice[i*2+1][2]]->position;
    }
    objects[0]->nb_springs = 0;
    objects[0]->springs = NULL;


    // dragon_model.obj 26000 faces
    // dragon_model2.obj 2000 faces
    // objects[2] = load_obj_file("objects/dragon_model.obj");
    objects[1] = load_obj_file("objects/suzanne.obj");

    point_t decalage = {3,0,1};
    float scale = 2;
    float theta = to_radians(10);
    point_t r1 = {scale, 0, 0};
    point_t r2 = {0, scale*cos(theta), -scale*sin(theta)};
    point_t r3 = {0, scale*sin(theta), scale*cos(theta)};
    for (int i=0; i<objects[1]->nb_sommets; i++) {
        point_t p = {
            r1.x*objects[1]->sommets[i]->position->x + r1.y*objects[1]->sommets[i]->position->y + r1.z*objects[1]->sommets[i]->position->z + decalage.x,
            r2.x*objects[1]->sommets[i]->position->x + r2.y*objects[1]->sommets[i]->position->y + r2.z*objects[1]->sommets[i]->position->z + decalage.y,
            r3.x*objects[1]->sommets[i]->position->x + r3.y*objects[1]->sommets[i]->position->y + r3.z*objects[1]->sommets[i]->position->z + decalage.z,
        };
        copy_point(p, objects[1]->sommets[i]->position);
    }
    compute_spring(objects[1], 4);
    printf("nb_sommet : %d, nb_springs : %d\n", objects[1]->nb_sommets, objects[1]->nb_springs);

    int nombre_triangle = 0;
    for (int i=0; i<nb_obj; i++) nombre_triangle += objects[i]->nb_triangles;
    triangle_t** triangles = malloc(sizeof(triangle_t*)*nombre_triangle);
    int k = 0;
    for (int i=0; i<nb_obj; i++) {
        for (int j=0; j<objects[i]->nb_triangles; j++) {
            triangles[k] = objects[i]->triangles[j];
            k++;
        }
    }

    objet_t* cube = load_obj_file("objects/cube.obj");

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

    SDL_bool simul = SDL_FALSE;
    SDL_bool simul2 = SDL_FALSE;

    SDL_bool show_help_message = SDL_TRUE;
    int nb_helpSurface = 0;
    SDL_Surface** surfaceHelpMessage = create_surfaces_from_text(liberation, help_message, white, &nb_helpSurface);
    SDL_Texture** textureHelpMessage = (SDL_Texture**) malloc(sizeof(SDL_Texture*)*nb_helpSurface);
    for (int i=0; i<nb_helpSurface; i++) {
        textureHelpMessage[i] = SDL_CreateTextureFromSurface(renderer, surfaceHelpMessage[i]);
    }
    SDL_Rect* help_message_rect = create_rect_from_text(liberation, help_message, nb_helpSurface);


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



        if (list_mem(keyDown, SDLK_g)) {
                keyDown = remove_list(keyDown, SDLK_g);
                if (simul) {
                    simul = SDL_FALSE;
                    simul2 = SDL_FALSE;
                } else simul = SDL_TRUE;
        }
        if (simul) {
            // force de graviter
            for (int i=0; i<objects[1]->nb_sommets; i++) {
                copy_point((point_t){0, sin(camera->roll), -1*cos(camera->roll)}, objects[1]->sommets[i]->acceleration);
            }

            // force des ressorts
            pthread_t threads[20];
            void* arg[20];
            int nb_threads = 20;
            int size = objects[1]->nb_springs/nb_threads;
            for (int i=0; i<nb_threads; i++) {
                arg[i] = malloc(sizeof(objet_t**)+2*sizeof(int*));
                *((objet_t**) arg[i]) = objects[1];
                *((int*) (arg[i]+sizeof(objet_t**))) = i*size;
                *((int*) (arg[i]+sizeof(objet_t**)+sizeof(int*))) = (i!=nb_threads)?(i+1)*size:objects[1]->nb_springs;
                int rc = pthread_create(&threads[i], NULL, force_ressort, arg[i]);
                if (rc){
                    printf("ERROR; return code from pthread_create() is %d\n", rc);
                    exit(-1);
                }
            }
            for (int i=0; i<nb_threads; i++) {
                pthread_join(threads[i], NULL);
                free(arg[i]);
            }

            if (!simul2) {
                simul2 = SDL_TRUE;
                for (int i=0; i<objects[1]->nb_sommets; i++) {
                    copy_point((point_t){0,0,0}, objects[1]->sommets[i]->acceleration);
                    copy_point((point_t){0,0,0}, objects[1]->sommets[i]->speed);
                }
            }

            // update speed / position
            for (int i=0; i<objects[1]->nb_sommets; i++) {
                float time_since_last_update = (float)(time_next - objects[1]->sommets[i]->time)/1000; // s
                objects[1]->sommets[i]->time = time_next;

                float coefficient_frottement = -0.2;
                copy_point(somme_point(produit_par_scalaire(coefficient_frottement, *objects[1]->sommets[i]->speed), *objects[1]->sommets[i]->acceleration), objects[1]->sommets[i]->acceleration);

                copy_point(somme_point(*objects[1]->sommets[i]->speed, produit_par_scalaire(time_since_last_update, *objects[1]->sommets[i]->acceleration)), objects[1]->sommets[i]->speed);

                float max_speed = 2;
                float norm_speed = norm(*objects[1]->sommets[i]->speed);
                if (norm_speed > max_speed) copy_point(produit_par_scalaire(max_speed/norm_speed, *objects[1]->sommets[i]->speed), objects[1]->sommets[i]->speed);

                copy_point(somme_point(*objects[1]->sommets[i]->position, produit_par_scalaire(time_since_last_update, *objects[1]->sommets[i]->speed)), objects[1]->sommets[i]->position);
            }

            // collision avec le sol
            for (int i=0; i<objects[1]->nb_sommets; i++) {
                if (objects[1]->sommets[i]->position->z < objects[0]->sommets[0]->position->z) {
                    objects[1]->sommets[i]->position->z = objects[0]->sommets[0]->position->z;
                    copy_point((point_t){0,0,0}, objects[1]->sommets[i]->acceleration);
                    copy_point((point_t){0,0,0}, objects[1]->sommets[i]->speed);
                }
            }
        }



        SDL_RenderClear(renderer);

        // float periode = 5; // s
        // float amplitude = 0.03; // space unit
        // mouvement de la forme A*cos(wt) + x0
        // donc vitesse de la forme -A*w*sin(wt)
        // float omega = 2 * M_PI * 1/periode;
        // float speed_amplitude = -amplitude*omega;
        // float t = (float)time_next/1000; // temps courant en seconde
        // point_t suzanne_speed = {speed_amplitude * SDL_sinf(omega * t), 0, 0};
        // move_obj(objects[2], &suzanne_speed);

        sort(camera, triangles, nombre_triangle);
        camera->triangle_pointer = NULL;
        for (int i=0; i<nombre_triangle; i++) {
            draw_triangle(camera, triangles[i], triangles[i]->color, triangles[i]->color, triangles[i]->color, window, renderer);
        }

        // Réticule
        float taille = 10;
        float epaisseur = 2;
        float w = screen_width*ratio;
        float h = screen_height*ratio;
        SDL_Color c = white;
        SDL_Vertex reticule1[3] = {
            {{w/2+taille/2, h/2+epaisseur/2}, c, {1, 1}}, 
            {{w/2+taille/2, h/2-epaisseur/2}, c, {1, 1}}, 
            {{w/2-taille/2, h/2-epaisseur/2}, c, {1, 1}}
        };
        SDL_RenderGeometry(renderer, NULL, reticule1, 3, NULL, 0);
        SDL_Vertex reticule2[3] = {
            {{w/2+taille/2, h/2+epaisseur/2}, c, {1, 1}}, 
            {{w/2-taille/2, h/2-epaisseur/2}, c, {1, 1}}, 
            {{w/2-taille/2, h/2+epaisseur/2}, c, {1, 1}}
        };
        SDL_RenderGeometry(renderer, NULL, reticule2, 3, NULL, 0);
        SDL_Vertex reticule3[3] = {
            {{w/2+epaisseur/2, h/2+taille/2}, c, {1, 1}}, 
            {{w/2+epaisseur/2, h/2-taille/2}, c, {1, 1}}, 
            {{w/2-epaisseur/2, h/2-taille/2}, c, {1, 1}}
        };
        SDL_RenderGeometry(renderer, NULL, reticule3, 3, NULL, 0);
        SDL_Vertex reticule4[3] = {
            {{w/2+epaisseur/2, h/2+taille/2}, c, {1, 1}}, 
            {{w/2-epaisseur/2, h/2-taille/2}, c, {1, 1}}, 
            {{w/2-epaisseur/2, h/2+taille/2}, c, {1, 1}}
        };
        SDL_RenderGeometry(renderer, NULL, reticule4, 3, NULL, 0);

        // Quand touche p enfoncée ajoute un cube
        if (camera->triangle_pointer != NULL) {
            // draw_triangle(camera, camera->triangle_pointer, white, white, white, window, renderer);
            if (list_mem(keyDown, SDLK_p)) {
                keyDown = remove_list(keyDown, SDLK_p);
                nb_obj++;
                objet_t** nobjs = (objet_t**) malloc(sizeof(objet_t*)*nb_obj);
                for (int i=0; i<nb_obj-1; i++) {
                    nobjs[i] = objects[i];
                }

                objet_t* new_cube = copy_obj(cube);

                point_t decalage = {0,0,0};
                decalage.x = (camera->triangle_pointer->p1->x + camera->triangle_pointer->p2->x + camera->triangle_pointer->p3->x)/3;
                decalage.y = (camera->triangle_pointer->p1->y + camera->triangle_pointer->p2->y + camera->triangle_pointer->p3->y)/3;
                decalage.z = (camera->triangle_pointer->p1->z + camera->triangle_pointer->p2->z + camera->triangle_pointer->p3->z)/3;
                point_t r3_non_normaliser = produit_vectoriel(soustraction_point(*camera->triangle_pointer->p2, *camera->triangle_pointer->p1), soustraction_point(*camera->triangle_pointer->p3, *camera->triangle_pointer->p1));
                point_t r3 = produit_par_scalaire(1/norm(r3_non_normaliser), r3_non_normaliser);
                point_t r2_non_normaliser = soustraction_point(*camera->point->position, decalage);
                r2_non_normaliser = soustraction_point(r2_non_normaliser, produit_par_scalaire(r3.x*r2_non_normaliser.x + r3.y*r2_non_normaliser.y + r3.z*r2_non_normaliser.z, r3));
                point_t r2 = produit_par_scalaire(1/norm(r2_non_normaliser), r2_non_normaliser);
                point_t r1 = produit_vectoriel(r2, r3);

                point_t position_decalage = soustraction_point(*camera->point->position, decalage);
                if (r3.x*position_decalage.x + r3.y*position_decalage.y + r3.z*position_decalage.z < 0) {
                    r1 = produit_par_scalaire(-1, r1);
                    r3 = produit_par_scalaire(-1, r3);
                }

                point_t r1_inv = {r2.y*r3.z-r2.z*r3.y, r1.z*r3.y-r1.y*r3.z, r1.y*r2.z-r1.z*r2.y};
                point_t r2_inv = {r2.z*r3.x-r2.x*r3.z, r1.x*r3.z-r1.z*r3.x, r1.z*r2.x-r1.x*r2.z};
                point_t r3_inv = {r2.x*r3.y-r2.y*r3.x, r1.y*r3.x-r1.x*r3.y, r1.x*r2.y-r1.y*r2.x};

                for (int i=0; i<new_cube->nb_sommets; i++) {
                    point_t p = {
                        r1_inv.x*new_cube->sommets[i]->position->x + r1_inv.y*new_cube->sommets[i]->position->y + r1_inv.z*new_cube->sommets[i]->position->z + decalage.x,
                        r2_inv.x*new_cube->sommets[i]->position->x + r2_inv.y*new_cube->sommets[i]->position->y + r2_inv.z*new_cube->sommets[i]->position->z + decalage.y,
                        r3_inv.x*new_cube->sommets[i]->position->x + r3_inv.y*new_cube->sommets[i]->position->y + r3_inv.z*new_cube->sommets[i]->position->z + decalage.z,
                    };
                    copy_point(p, new_cube->sommets[i]->position);
                }
                
                point_t dec2 = produit_par_scalaire(0.5, r3);
                move_obj(new_cube, &dec2);

                nobjs[nb_obj-1] = new_cube;
                free(objects);
                objects = nobjs;
                nombre_triangle = 0;
                for (int i=0; i<nb_obj; i++) nombre_triangle += objects[i]->nb_triangles;
                free(triangles);
                triangles = malloc(sizeof(triangle_t*)*nombre_triangle);
                int k = 0;
                for (int i=0; i<nb_obj; i++) {
                    for (int j=0; j<objects[i]->nb_triangles; j++) {
                        triangles[k] = objects[i]->triangles[j];
                        k++;
                    }
                }
            }
        }

        if (list_mem(keyDown, SDLK_h)) {
            keyDown = remove_list(keyDown, SDLK_h);
            show_help_message = !show_help_message;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
        if (show_help_message) {
            for (int i=0; i<nb_helpSurface; i++) {
                SDL_RenderCopy(renderer, textureHelpMessage[i], NULL, &help_message_rect[i]);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(Message);

    }

    free(timetext);

    for (int i=0; i<nb_helpSurface; i++) {
        SDL_FreeSurface(surfaceHelpMessage[i]);
        SDL_DestroyTexture(textureHelpMessage[i]);
    }
    free(surfaceHelpMessage);
    free(help_message_rect);
    free(textureHelpMessage);

    free(time_elapsed);
    free(triangles);
    for (int i=0; i<nb_obj; i++) {
        free_obj(objects[i]);
    }
    free(objects);

    free_obj(cube);

    destroy_camera(camera);
    TTF_CloseFont(liberation);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
