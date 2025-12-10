#include "func.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define M_PI 3.14159265358979323846

// 각도를 라디안으로 변환
static double deg2rad(int deg) {
    return (double)deg * M_PI / 180.0;
}

// 각도를 [0,359] 범위로 정규화
static int norm_deg(int deg) {
    int r = deg % 360;
    if (r < 0) r += 360;
    return r;
}

// 정수 각도 입력에 대한 sin 계산 (double 반환)
static double sin_deg(int deg) {
    int d = norm_deg(deg);
    double r = sin(deg2rad(d));
    return r; // 소수값 그대로 반환
}

// 정수 각도 입력에 대한 cos 계산 (double 반환)
static double cos_deg(int deg) {
    int d = norm_deg(deg);
    double r = cos(deg2rad(d));
    return r;
}

int do_func(const char* func_name, int val, int* ok) {
    if (strcmp(func_name, "sin") == 0) {
        double res = sin_deg(val);
        // 소수점 3자리까지 출력
        printf("%.3f\n", res);
        if (ok) *ok = 2; // 이미 출력됨을 표시
        return (int)round(res);
    }
    if (strcmp(func_name, "cos") == 0) {
        double res = cos_deg(val);
        printf("%.3f\n", res);
        if (ok) *ok = 2;
        return (int)round(res);
    }
    if (strcmp(func_name, "sqrt") == 0) {
        if (val < 0) { printf("syntax error\n"); if (ok) *ok = 0; return 0; }
        double res = sqrt((double)val);
        printf("%.3f\n", res);
        if (ok) *ok = 2;
        return (int)round(res);
    }
    if (strcmp(func_name, "abs") == 0) { if (ok) *ok = 1; return abs(val); }
    if (strcmp(func_name, "int") == 0) { if (ok) *ok = 1; return val; }
    if (strcmp(func_name, "power") == 0) { if (ok) *ok = 1; return val * val; }
    if (strcmp(func_name, "pow") == 0) { if (ok) *ok = 1; return val * val; }
    if (strcmp(func_name, "random") == 0) {
        static int seeded = 0;
        if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
        if (val <= 0) { printf("syntax error\n"); if (ok) *ok = 0; return 0; }
        if (ok) *ok = 1; return rand() % (val + 1);
    }

    printf("command error\n"); if (ok) *ok = 0; return val;
}

// 정수를 이진 문자열로 출력
void print_bin(int n) {
    if (n == 0) { printf("0b0\n"); return; }
    char buffer[33];
    buffer[32] = '\0';
    int i = 31;
    unsigned int un = (unsigned int)n;
    if (un == 0) { printf("0b0\n"); return; }
    while (un > 0 && i >= 0) {
        buffer[i--] = (un & 1) ? '1' : '0';
        un >>= 1;
    }
    printf("0b%s\n", &buffer[i + 1]);
}

// 정수를 16진수로 출력
void print_hex(int n) {
    printf("0x%X\n", n);
}

// 정수를 8진수로 출력
void print_oct(int n) {
    printf("0%o\n", n);
}