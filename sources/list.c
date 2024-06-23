#include "../includes/list.h"

list_t* add_list(list_t* l, int v) {
    list_t* nl = (list_t*) malloc(sizeof(list_t));
    nl->val = v;
    nl->next = l;
    return nl;
}

SDL_bool list_mem(list_t* l, int v) {
    while (l != NULL) {
        if (l->val == v) return SDL_TRUE;
        l = l->next;
    }
    return SDL_FALSE;
}

list_t* remove_list(list_t* l, int v) {
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