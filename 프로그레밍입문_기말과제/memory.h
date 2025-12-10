#pragma once

#define MAX_VARS 50
#define VAR_NAME_LEN 4

typedef struct {
    char name[VAR_NAME_LEN];
    int value;
} Variable;

void init_memory();
void set_var(const char* name, int value);
int get_var(const char* name); // 못 찾으면 에러 처리 필요
void print_all_vars();
void set_ret(int val);
int get_ret();
int is_existing_var(const char* name); // 변수 존재 여부 확인