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

typedef struct {
    point_t p1, p2, p3;
    SDL_Color color;
} triangle_t;

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
    *y = -(2 * phi / fov_vertical) * h + 0.5*h;
}

point_t produit_vectoriel(point_t p1, point_t p2) {
    point_t p = {p1.y*p2.z - p1.z*p2.y, p1.z*p2.x - p1.x*p2.z, p1.x*p2.y - p1.y*p2.x};
    return p;
}

float dot(point_t p1, point_t p2) {
    return p1.x*p2.x + p1.y*p2.y + p1.z*p2.z;
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

triangle_t* init_triangle(point_t p1, point_t p2, point_t p3, SDL_Color c) {
    triangle_t* t = malloc(sizeof(triangle_t));
    t->color = c;
    t->p1 = p1;
    t->p2 = p2;
    t->p3 = p3;
    return t;
}

float distance(triangle_t* t, camera_t* camera) {
    float dx = t->p1.x - camera->x;
    float dy = t->p1.y - camera->y;
    float dz = t->p1.z - camera->z;
    float dp1 = sqrt(dx*dx + dy*dy + dz*dz);

    dx = t->p2.x - camera->x;
    dy = t->p2.y - camera->y;
    dz = t->p2.z - camera->z;
    float dp2 = sqrt(dx*dx + dy*dy + dz*dz);

    dx = t->p3.x - camera->x;
    dy = t->p3.y - camera->y;
    dz = t->p3.z - camera->z;
    float dp3 = sqrt(dx*dx + dy*dy + dz*dz);
    return (dp1+dp2+dp3)/3;
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

triangle_t* load_obj_file(char* filename, int* nbt) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Pas de fichier %s\n", filename);
    }
    int nb_sommet = 0;
    int nb_triangle = 0;
    int nb_vt = 0;
    int nb_vn = 0;
    char* c = malloc(sizeof(char)*20);
    while (fscanf(file, "%s", c) != EOF) {
        if (strcmp(c, "v") == 0) nb_sommet++;
        else if (strcmp(c, "f") == 0) nb_triangle++;
        else if (strcmp(c, "vt") == 0) nb_vt++;
        else if (strcmp(c, "vn") == 0) nb_vn++;
    }
    free(c);
    fclose(file);
    file = fopen(filename, "r");
    point_t* points = (point_t*) malloc(sizeof(point_t)*nb_sommet);
    triangle_t* triangles = (triangle_t*) malloc(sizeof(triangle_t)*nb_triangle);
    char* c1 = malloc(sizeof(char)*200);
    int is = 0;
    int it = 0;

    while (fscanf(file, "%s", c1) != EOF) {
        if (strcmp(c1, "v") == 0) {
            float x,y,z;
            fscanf(file, "%f %f %f", &x, &y, &z);
            points[is].x = x;
            points[is].y = y;
            points[is].z = z;
            is++;
        } else if (strcmp(c1, "f") == 0) {
            int v1, vt1, vn1;
            int v2, vt2, vn2;
            int v3, vt3, vn3;
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
            triangles[it].color.a = 255;
            triangles[it].color.r = rand()%255;
            triangles[it].color.g = rand()%255;
            triangles[it].color.b = rand()%255;
            triangles[it].p1 = points[v1-1];
            triangles[it].p2 = points[v2-1];
            triangles[it].p3 = points[v3-1];
            it++;
        } else {
            fscanf(file, "%[^\n]", c1);
        }
    }
    fclose(file);
    free(points);
    free(c1);
    *nbt = nb_triangle;
    return triangles;
}

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
                    //printf("%f\n", sqrtf(sin(camera.yaw) * cos(camera.pitch) * sin(camera.yaw) * cos(camera.pitch) + cos(camera.pitch) * cos(camera.yaw) * cos(camera.pitch) * cos(camera.yaw) + sin(camera.pitch) * sin(camera.pitch)));
                    break;

                case SDL_MOUSEWHEEL : 
                    //camera.yaw -= event.wheel.preciseX * camera.sensitivity;
                    //camera.pitch -= event.wheel.preciseY * camera.sensitivity;
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
