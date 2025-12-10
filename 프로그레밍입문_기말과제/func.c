#include "func.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define M_PI 3.14159265358979323846

// Convert degrees to radians
static double deg2rad(int deg) {
    return (double)deg * M_PI / 180.0;
}

// Normalize degree to [0,359]
static int norm_deg(int deg) {
    int r = deg % 360;
    if (r < 0) r += 360;
    return r;
}

// Integer sin for degree input: compute and round for all angles
static int sin_deg(int deg) {
    int d = norm_deg(deg);
    double r = sin(deg2rad(d));
    return (int)round(r); // round to nearest integer (portable on Windows)
}

// Integer cos for degree input: compute and round for all angles
static int cos_deg(int deg) {
    int d = norm_deg(deg);
    double r = cos(deg2rad(d));
    return (int)round(r);
}

int do_func(const char* func_name, int val) {
    if (strcmp(func_name, "sin") == 0) {
        return sin_deg(val);
    }
    if (strcmp(func_name, "cos") == 0) {
        return cos_deg(val);
    }
    if (strcmp(func_name, "sqrt") == 0) {
        if (val < 0) { printf("Error: Domain error.\n"); return 0; }
        return (int)round(sqrt((double)val));
    }
    if (strcmp(func_name, "abs") == 0) return abs(val);
    if (strcmp(func_name, "int") == 0) return val;
    if (strcmp(func_name, "power") == 0) return val * val;
    if (strcmp(func_name, "pow") == 0) return val * val;
    if (strcmp(func_name, "random") == 0) {
        static int seeded = 0;
        if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
        if (val <= 0) return 0;
        return rand() % (val + 1);
    }

    printf("Error: Unknown function '%s'\n", func_name);
    return val;
}

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

void print_hex(int n) {
    printf("0x%X\n", n);
}

void print_oct(int n) {
    printf("0%o\n", n);
}