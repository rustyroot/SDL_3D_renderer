#include "../includes/list.h"

int_list_t* add_int_list(int_list_t* l, int v) {
    int_list_t* nl = (int_list_t*) malloc(sizeof(int_list_t));
    nl->val = v;
    nl->next = l;
    return nl;
}

SDL_bool int_list_mem(int_list_t* l, int v) {
    while (l != NULL) {
        if (l->val == v) return SDL_TRUE;
        l = l->next;
    }
    return SDL_FALSE;
}

int_list_t* remove_int_list(int_list_t* l, int v) {
    if (l == NULL) return l;
    else if (l->val == v) {
        int_list_t* ln = l->next;
        free(l);
        return remove_int_list(ln, v);
    } else {
        int_list_t* ln = remove_int_list(l->next, v);
        l->next = ln;
        return l;
    }
}

int int_list_size(int_list_t* l) {
    int n = 0;
    while (l != NULL) {
        n++;
        l = l->next;
    }
    return n;
}


list_t* add_list(list_t* l, void* v) {
    list_t* nl = (list_t*) malloc(sizeof(list_t));
    nl->val = v;
    nl->next = l;
    return nl;
}

SDL_bool list_mem(list_t* l, void* v) {
    while (l != NULL) {
        if (l->val == v) return SDL_TRUE;
        l = l->next;
    }
    return SDL_FALSE;
}

list_t* remove_list(list_t* l, void* v) {
    if (l == NULL) return l;
    else if (l->val == v) {
        list_t* ln = l->next;
        free(l);
        return remove_list(ln, v);
    } else {
        list_t* ln = remove_list(l->next, v);
        l->next = ln;
        return l;
    }
}

int list_size(list_t* l) {
    int n = 0;
    while (l != NULL) {
        n++;
        l = l->next;
    }
    return n;
}

void free_list(list_t* l, void* free_fnc(void* el)) {
    while (l != NULL) {
        if (free_fnc != NULL) free_fnc(l->val);
        list_t* ln = l->next;
        free(l);
        l = ln;
    }
}