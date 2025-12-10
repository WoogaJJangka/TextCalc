#define _CRT_SECURE_NO_WARNINGS
#include "eval.h"
#include "memory.h"
#include "func.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// 문자열이 정수인지 확인하는 헬퍼
int is_number(const char* s) {
    if (*s == '-' || *s == '+') s++;
    if (!*s) return 0;
    while (*s) {
        if (!isdigit(*s)) return 0;
        s++;
    }
    return 1;
}

// 값 가져오기 (숫자면 atoi, 문자면 변수값)
int get_value(const char* token) {
    if (is_number(token)) return atoi(token);
    return get_var(token);
}

int evaluate(char* input) {
    char t1[50] = { 0 }, op[10] = { 0 }, t2[50] = { 0 };

    int n = sscanf_s(input, "%s %s %s", t1, (unsigned)sizeof(t1), op, (unsigned)sizeof(op), t2, (unsigned)sizeof(t2));

    if (n == 3) {
        int v1 = get_value(t1);
        int v2 = get_value(t2);

        if (strcmp(op, "+") == 0) return v1 + v2;
        if (strcmp(op, "-") == 0) return v1 - v2;
        if (strcmp(op, "*") == 0) return v1 * v2;
        if (strcmp(op, "/") == 0) {
            if (v2 == 0) { printf("Error: Division by zero\n"); return 0; }
            return v1 / v2;
        }
        if (strcmp(op, "%") == 0) {
            if (v2 == 0) { printf("Error: Division by zero\n"); return 0; }
            return v1 % v2;
        }
        if (strcmp(op, "^") == 0) {
            int res = 1;
            int base = v1;
            int exp = v2;
            if (exp < 0) { printf("Error: Negative exponent for integer pow\n"); return 0; }
            while (exp > 0) { if (exp & 1) res *= base; base *= base; exp >>= 1; }
            return res;
        }
    }

    if (n == 2) {
        int v2 = get_value(op);

        if (strcmp(t1, "sin") == 0 || strcmp(t1, "cos") == 0 ||
            strcmp(t1, "sqrt") == 0 || strcmp(t1, "abs") == 0 || strcmp(t1, "int") == 0) {
            return do_func(t1, v2);
        }

        if (strcmp(t1, "bin") == 0) { print_bin(v2); return v2; }
        if (strcmp(t1, "hex") == 0) { print_hex(v2); return v2; }
        if (strcmp(t1, "oct") == 0) { print_oct(v2); return v2; }
    }

    if (n == 1) {
        return get_value(t1);
    }

    printf("Error: Invalid Syntax\n");
    return 0;
}