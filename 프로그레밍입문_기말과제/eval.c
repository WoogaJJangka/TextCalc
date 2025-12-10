#define _CRT_SECURE_NO_WARNINGS
#include "eval.h"
#include "memory.h"
#include "func.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

// 문자열이 정수인지 확인하는 헬퍼 함수
int is_number(const char* s) {
    if (*s == '-' || *s == '+') s++;
    if (!*s) return 0;
    while (*s) {
        if (!isdigit(*s)) return 0;
        s++;
    }
    return 1;
}

// 접두사(0b, 0x, 0) 기반 정수 리터럴을 파싱하여 10진수 값을 반환
// 허용 형식: 0b..., 0B..., 0x..., 0X..., 0..., 또는 일반 10진수
// 형식이 잘못되었으면 ok를 0으로 설정하고 0을 반환
int parse_int_literal_str(const char* s, int* ok) {
    if (ok) *ok = 1;
    if (!s || !*s) { if (ok) *ok = 0; return 0; }
    const char* p = s;
    int sign = 1;
    if (*p == '+' || *p == '-') {
        if (*p == '-') sign = -1;
        p++;
        if (!*p) { if (ok) *ok = 0; return 0; }
    }
    // 2진수 접두사 0b
    if (p[0] == '0' && (p[1] == 'b' || p[1] == 'B')) {
        p += 2;
        if (!*p) { if (ok) *ok = 0; return 0; }
        int v = 0;
        while (*p) {
            if (*p != '0' && *p != '1') { if (ok) *ok = 0; return 0; }
            v = (v << 1) + (*p - '0');
            p++;
        }
        return sign * v;
    }
    // 16진수 접두사 0x
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
        if (!*p) { if (ok) *ok = 0; return 0; }
        char* endptr = NULL;
        long v = strtol(p, &endptr, 16);
        if (*endptr != '\0') { if (ok) *ok = 0; return 0; }
        return sign * (int)v;
    }
    // 8진수: 선행 0이고 다음 문자가 숫자일 때 (예: 073)
    if (p[0] == '0' && isdigit((unsigned char)p[1])) {
        char* endptr = NULL;
        long v = strtol(p, &endptr, 8);
        if (*endptr != '\0') { if (ok) *ok = 0; return 0; }
        return sign * (int)v;
    }
    // 일반 10진수
    char* endptr = NULL;
    long v = strtol(p, &endptr, 10);
    if (*endptr != '\0') { if (ok) *ok = 0; return 0; }
    return sign * (int)v;
}

// 토큰에서 값을 가져옴 (숫자면 atoi 또는 접두사 처리, 그렇지 않으면 변수값 조회)
// 변수 미정의일 경우 ok가 제공되면 0으로 설정하고 에러 메시지 출력
int get_value(const char* token, int* ok) {
    if (is_number(token)) return atoi(token);
    // 접두사 형태의 리터럴도 허용 (0x, 0b, 0...)
    int lit_ok = 1;
    int lit_val = parse_int_literal_str(token, &lit_ok);
    if (lit_ok) return lit_val;
    // 변수인지 확인
    if (!is_existing_var(token)) {
        printf("not define variable\n");
        if (ok) *ok = 0;
        return 0;
    }
    return get_var(token);
}

// 헬퍼: 인수 문자열이 접두사 기반 리터럴인지 검사(부호 허용)
static int is_prefixed_literal(const char* s) {
    if (!s || !*s) return 0;
    const char* p = s;
    if (*p == '+' || *p == '-') p++;
    if (!*p) return 0;
    if (p[0] == '0') {
        if (p[1] == 'x' || p[1] == 'X') return 1;
        if (p[1] == 'b' || p[1] == 'B') return 1;
        if (isdigit((unsigned char)p[1])) return 1; // 0으로 시작하는 8진수
        if (p[1] == '\0') return 1; // 단일 문자 '0'
    }
    return 0;
}

// 입력 문자열을 평가하고, 성공 여부는 ok 포인터에 기록함
int evaluate(char* input, int* ok) {
    if (ok) *ok = 1; // 기본적으로 성공으로 가정

    char t1[50] = { 0 }, op[50] = { 0 }, t2[50] = { 0 };
    int n = 0;

    // 함수 형태의 입력을 지원: 예) int(0x14) 또는 sin(90)
    char* lp = strchr(input, '(');
    char* rp = strchr(input, ')');
    if (lp && rp && rp > lp) {
        // 함수 이름 추출
        size_t fn_len = lp - input;
        while (fn_len > 0 && isspace((unsigned char)input[fn_len-1])) fn_len--;
        if (fn_len >= sizeof(t1)) fn_len = sizeof(t1)-1;
        strncpy(t1, input, fn_len);
        t1[fn_len] = '\0';
        // 인수 추출
        size_t arg_len = rp - (lp + 1);
        if (arg_len >= sizeof(op)) arg_len = sizeof(op)-1;
        strncpy(op, lp + 1, arg_len);
        op[arg_len] = '\0';
        n = 2;
    } else {
        n = sscanf_s(input, "%49s %49s %49s", t1, (unsigned)sizeof(t1), op, (unsigned)sizeof(op), t2, (unsigned)sizeof(t2));
    }

    if (n == 3) {
        int v1 = get_value(t1, ok);
        if (ok && *ok == 0) return 0;
        int v2 = get_value(t2, ok);
        if (ok && *ok == 0) return 0;

        if (strcmp(op, "+") == 0) return v1 + v2;
        if (strcmp(op, "-") == 0) return v1 - v2;
        if (strcmp(op, "*") == 0) return v1 * v2;
        if (strcmp(op, "/") == 0) {
            if (v2 == 0) { printf("syntax error\n"); if (ok) *ok = 0; return 0; }
            return v1 / v2;
        }
        if (strcmp(op, "%") == 0) {
            if (v2 == 0) { printf("syntax error\n"); if (ok) *ok = 0; return 0; }
            return v1 % v2;
        }
        if (strcmp(op, "^") == 0) {
            int res = 1;
            int base = v1;
            int exp = v2;
            if (exp < 0) { printf("syntax error\n"); if (ok) *ok = 0; return 0; }
            while (exp > 0) { if (exp & 1) res *= base; base *= base; exp >>= 1; }
            return res;
        }
    }

    if (n == 2) {
        // int 함수는 입력 형식(0b, 0x, 0 등)만 허용
        if (strcmp(t1, "int") == 0) {
            if (!is_prefixed_literal(op)) { printf("invalid literal\n"); if (ok) *ok = 0; return 0; }
            int lit_ok = 1;
            int v = parse_int_literal_str(op, &lit_ok);
            if (!lit_ok) { printf("invalid literal\n"); if (ok) *ok = 0; return 0; }
            printf("%d\n", v);
            return v;
        }

        int v2 = get_value(op, ok);
        if (ok && *ok == 0) return 0;

        // 함수 호출 처리 (int는 위에서 이미 처리함). 함수 내부에서 오류는 func_ok로 전달
        if (strcmp(t1, "sin") == 0 || strcmp(t1, "cos") == 0 ||
            strcmp(t1, "sqrt") == 0 || strcmp(t1, "abs") == 0) {
            int func_ok = 1;
            int r = do_func(t1, v2, &func_ok);
            if (!func_ok) { if (ok) *ok = 0; return 0; }
            return r;
        }

        if (strcmp(t1, "bin") == 0) { print_bin(v2); return v2; }
        if (strcmp(t1, "hex") == 0) { print_hex(v2); return v2; }
        if (strcmp(t1, "oct") == 0) { print_oct(v2); return v2; }
        if (strcmp(t1, "random") == 0 || strcmp(t1, "power") == 0 || strcmp(t1, "pow") == 0) {
            int func_ok = 1;
            int r = do_func(t1, v2, &func_ok);
            if (!func_ok) { if (ok) *ok = 0; return 0; }
            return r;
        }
    }

    if (n == 1) {
        return get_value(t1, ok);
    }

    // 형식이 맞지 않을 때 에러 메시지 출력
    printf("syntax error\n"); if (ok) *ok = 0; return 0;
}