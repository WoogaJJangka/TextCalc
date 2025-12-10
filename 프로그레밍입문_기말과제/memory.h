#pragma once
/* memory.h */
#ifndef MEMORY_H
#define MEMORY_H

#define MAX_VARS 50

typedef struct {
    char name[4];   // 최대 3글자 + 널문자
    int value;
    int used;       // 0: 비어있음, 1: 사용중
} Var;

void init_memory(void);
int set_var(const char* name, int value);      // 성공: 0, 실패: -1 (예: 자리없음)
int get_var(const char* name, int* out_value); // 성공: 0, 미정의: -1
int del_var(const char* name);                 // 성공: 0, 없음: -1
void print_var(const char* name);
void print_all(void);

extern int ret_value;  // 바로 전 계산 결과를 저장하는 전역 변수

#endif
