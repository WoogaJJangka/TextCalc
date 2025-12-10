#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "memory.h"
#include "eval.h"
#include "func.h"

// 문자열 좌우 공백 제거
static char* trim(char* s) {
    char* end;
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

// 변수명 유효성 검사: 영문자만, 길이 1~3, 예약어 아님
static int is_reserved(const char* name) {
    const char* reserved[] = {
        "set","print","del","ret",
        "abs","sqrt","sin","cos","power","random","bin","hex","oct","int","pow",
        NULL
    };
    for (int i = 0; reserved[i]; i++) {
        if (strcmp(name, reserved[i]) == 0) return 1;
    }
    return 0;
}

static int is_valid_varname(const char* name) {
    size_t len = strlen(name);
    if (len == 0 || len > 3) return 0;
    for (size_t i = 0; i < len; i++) {
        if (!isalpha((unsigned char)name[i])) return 0;
    }
    if (is_reserved(name)) return 0;
    return 1;
}

// 0b, 0x, 0 접두사 파싱, 선택적 +/- 지원
static int parse_int_literal(const char* s) {
    if (!s || !*s) return 0;
    char buf[256];
    strncpy(buf, s, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    char* p = buf;
    int sign = 1;
    if (*p == '+' || *p == '-') {
        if (*p == '-') sign = -1;
        p++;
    }
    if (p[0] == '0' && (p[1] == 'b' || p[1] == 'B')) {
        // 이진수
        p += 2;
        int v = 0;
        while (*p == '0' || *p == '1') { v = (v<<1) + (*p - '0'); p++; }
        return sign * v;
    }
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        // 16진수
        long v = strtol(p+2, NULL, 16);
        return sign * (int)v;
    }
    if (p[0] == '0' && isdigit((unsigned char)p[1])) {
        // 8진수
        long v = strtol(p, NULL, 8);
        return sign * (int)v;
    }
    // 10진수
    long v = strtol(p, NULL, 10);
    return sign * (int)v;
}

int main() {
    char input[256];

    init_memory();

    printf("=== VS C-Calculator ===\n");
    printf("Commands: set <var> = <expr>, print <var|all|ret>, del <var>, exit\n");
    printf("Examples: set a = 10, print a, del a, 3 + 5, sin 30\n\n");

    while (1) {
        printf(":> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) return 0;
            return 1;
        }
        // 개행 제거
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') input[len-1] = '\0';
        char copy[256];
        strcpy_s(copy, sizeof(copy), input);
        char* line = trim(copy);
        if (line[0] == '\0') continue;

        // copy를 토큰화하여 명령 파싱
        char work[256];
        strcpy_s(work, sizeof(work), line);
        char* tok = strtok(work, " ");
        if (!tok) continue;

        if (strcmp(tok, "exit") == 0) break;
        else if (strcmp(tok, "print") == 0) {
            char* arg = strtok(NULL, " ");
            if (!arg) { printf("Usage: print <var|all|ret>\n"); continue; }
            if (strcmp(arg, "all") == 0) { print_all_vars(); continue; }
            if (strcmp(arg, "ret") == 0) { printf("%d\n", get_ret()); continue; }
            if (!is_valid_varname(arg)) { printf("Invalid variable name.\n"); continue; }
            if (!is_existing_var(arg)) { printf("not define variable\n"); continue; }
            printf("%d\n", get_var(arg));
            continue;
        }
        else if (strcmp(tok, "del") == 0) {
            char* var = strtok(NULL, " ");
            if (!var) { printf("Usage: del <var>\n"); continue; }
            if (!is_valid_varname(var)) { printf("Invalid variable name.\n"); continue; }
            del_var(var);
            continue;
        }
        else if (strcmp(tok, "set") == 0) {
            // 지원: set a = expr 또는 set a expr
            char* var = strtok(NULL, " ");
            if (!var) { printf("Usage: set <var> = <expr>\n"); continue; }
            if (!is_valid_varname(var)) { printf("Invalid variable name.\n"); continue; }
            // 원본 줄에서 '= ' 찾기
            char* eq = strchr(line, '=');
            int value = 0;
            if (eq) {
                char* expr = trim(eq + 1);
                if (expr[0] == '\0') { printf("Usage: set <var> = <expr>\n"); continue; }
                int ok = 1;
                value = evaluate(expr, &ok);
                if (!ok) { /* do not set variable on error */ continue; }
            } else {
                // '='가 없으면 다음 토큰을 식/값으로 사용
                char* valtok = strtok(NULL, ""); // 문자열의 나머지
                if (!valtok) { printf("Usage: set <var> = <expr>\n"); continue; }
                valtok = trim(valtok);
                // 먼저 리터럴로 파싱하고, 아니면 evaluate 사용
                // determine if valtok is a plain integer literal
                int is_lit = 1;
                const char* pch = valtok;
                if (*pch == '+' || *pch == '-') pch++;
                if (*pch == '\0') is_lit = 0;
                while (*pch) {
                    if (!(isdigit((unsigned char)*pch) || *pch=='x'||*pch=='X'||*pch=='b'||*pch=='B'||(*pch>='a'&&*pch<='f')||(*pch>='A'&&*pch<='F')||*pch=='0')) { is_lit = 0; break; }
                    pch++;
                }
                if (is_lit) {
                    value = parse_int_literal(valtok);
                } else {
                    int ok = 1;
                    value = evaluate(valtok, &ok);
                    if (!ok) { continue; }
                }
            }
            set_var(var, value);
            printf("Set %s = %d\n", var, value);
            continue;
        }

        // 그 외에는 표현식으로 처리
        int ok = 1;
        int result = evaluate(line, &ok);
        if (ok == 1) {
            set_ret(result);
            printf("= %d\n", result);
        } else if (ok == 2) {
            // 함수에서 이미 포맷된 출력을 했음. ret은 반환값(반올림된 정수)으로 설정
            set_ret(result);
        }
    }

    return 0;
}