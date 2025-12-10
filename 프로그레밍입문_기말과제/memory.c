/* memory.c */
#include <stdio.h>
#include <string.h>
#include "memory.h"

static Var vars[MAX_VARS];
int ret_value = 0;

void init_memory(void) {
    for (int i = 0; i < MAX_VARS; i++) {
        vars[i].used = 0;
    }
    ret_value = 0;
}

static int find_index(const char* name) {
    for (int i = 0; i < MAX_VARS; i++) {
        if (vars[i].used && strcmp(vars[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int set_var(const char* name, int value) {
    int idx = find_index(name);
    if (idx >= 0) {
        vars[idx].value = value;
        return 0;
    }
    for (int i = 0; i < MAX_VARS; i++) {
        if (!vars[i].used) {
            {
                size_t j;
                for (j = 0; j < sizeof(vars[i].name) - 1 && name[j] != '\0'; j++) {
                    vars[i].name[j] = name[j];
                }
                vars[i].name[j] = '\0';
            }
            vars[i].value = value;
            vars[i].used = 1;
            return 0;
        }
    }
    return -1; // 공간 없음
}

int get_var(const char* name, int* out_value) {
    int idx = find_index(name);
    if (idx < 0) return -1;
    *out_value = vars[idx].value;
    return 0;
}

int del_var(const char* name) {
    int idx = find_index(name);
    if (idx < 0) return -1;
    vars[idx].used = 0;
    return 0;
}

void print_var(const char* name) {
    int v;
    if (strcmp(name, "ret") == 0) {
        printf("%d\n", ret_value);
        return;
    }
    if (get_var(name, &v) == 0) {
        printf("%d\n", v);
    }
    else {
        printf("not define variable\n");
    }
}

void print_all(void) {
    for (int i = 0; i < MAX_VARS; i++) {
        if (vars[i].used) {
            printf("%s = %d\n", vars[i].name, vars[i].value);
        }
    }
}
