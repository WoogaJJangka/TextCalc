#include "memory.h"
#include <stdio.h>
#include <string.h>

static Variable vars[MAX_VARS];
static int var_count = 0;
static int ret_val = 0;

void init_memory() {
    var_count = 0;
    ret_val = 0;
}

void set_var(const char* name, int value) {
    // 기존 변수 업데이트
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            vars[i].value = value;
            return;
        }
    }

    // 새 변수 추가
    if (var_count < MAX_VARS) {
        // 이름은 최대 VAR_NAME_LEN-1 까지 안전하게 복사
        size_t j;
        for (j = 0; j < (size_t)(VAR_NAME_LEN - 1) && name[j] != '\0'; j++) {
            vars[var_count].name[j] = name[j];
        }
        vars[var_count].name[j] = '\0';
        vars[var_count].value = value;
        var_count++;
    }
    else {
        printf("Error: Memory full.\n");
    }
}

int get_var(const char* name) {
    if (strcmp(name, "ret") == 0) return ret_val;

    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].value;
        }
    }
    printf("not define variable\n");
    return 0;
}

void del_var(const char* name) {
    // 변수 찾기
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            // 뒤의 항목을 앞으로 당겨 덮어쓰기
            for (int j = i; j < var_count - 1; j++) {
                vars[j] = vars[j + 1];
                printf("variable %s deleted\n",vars[i].name);
            }
            var_count--;
            return;
        }
    }
    printf("not define variable\n");
}

void print_all_vars() {
    printf("--- Variable Memory ---\n");
    for (int i = 0; i < var_count; i++) {
        printf("%s = %d\n", vars[i].name, vars[i].value);
    }
    printf("ret = %d\n", ret_val);
    printf("-----------------------\n");
}

void set_ret(int val) {
    ret_val = val;
}

int get_ret() {
    return ret_val;
}

int is_existing_var(const char* name) {
    if (strcmp(name, "ret") == 0) return 1;
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) return 1;
    }
    return 0; // 사용할 수 있는 변수
}