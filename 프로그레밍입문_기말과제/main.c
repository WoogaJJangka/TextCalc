/* main.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory.h"

#define LINE_MAX 256

int main(void) {
    char line[LINE_MAX];

    init_memory();

    while (1) {
        printf(":> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break; // EOF
        }

        // 줄 끝 개행 제거
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        // 간단한 종료 명령 예시
        if (strcmp(line, "exit") == 0) {
            break;
        }

        // 첫 토큰으로 명령 구분
        char* cmd = strtok(line, " ");
        if (cmd == NULL) continue;

        if (strcmp(cmd, "set") == 0) {
            // 형식: set 이름 = 값   (일단 10진수 값만)
            char* name = strtok(NULL, " ");
            char* eq = strtok(NULL, " ");
            char* val = strtok(NULL, " ");

            if (!name || !eq || !val || strcmp(eq, "=") != 0) {
                printf("syntax error\n");
                continue;
            }

            int v = atoi(val);  // 추후: 진법 파싱 함수로 교체
            if (set_var(name, v) != 0) {
                printf("command error\n"); // 예: 메모리 풀
            }

        }
        else if (strcmp(cmd, "print") == 0) {
            // print a / print all / print ret
            char* arg = strtok(NULL, " ");
            if (!arg) {
                printf("syntax error\n");
                continue;
            }
            if (strcmp(arg, "all") == 0) {
                print_all();
            }
            else {
                print_var(arg);
            }

        }
        else if (strcmp(cmd, "del") == 0) {
            char* name = strtok(NULL, " ");
            if (!name) {
                printf("syntax error\n");
                continue;
            }
            if (del_var(name) != 0) {
                printf("not define variable\n");
            }

        }
        else {
            // 앞으로 이 부분에: 이항 연산 / 함수 호출 파싱을 넣을 예정
            printf("command error\n");
        }
    }

    return 0;
}
