#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "eval.h"
#include "func.h"

int main() {
    char input[256];

    init_memory();

    printf("=== VS C-Calculator ===\n");
    printf("Commands: set <var> <val>, print, del <var>, exit\n");
    printf("Examples: 3 + 5, sin 30, x * 2\n\n");

    while (1) {
        printf(":> ");

        // 입력이 비정상이면 요류 출력 및 종
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                // EOF(예: Ctrl+D 또는 입력 스트림 종료)
                fprintf(stderr, "종료: 입력 스트림의 끝(EOF) 감지\n");
                return 0;
            }
            else if (ferror(stdin)) {
                // 읽기 오류가 발생한 경우 자세한 원인 출력
                perror("입력 오류");
                return 1;
            }
            else {
                // 일반적이지 않은 NULL 반환의 경우
                fprintf(stderr, "알 수 없는 입력 오류로 종료\n");
                return 1;
            }
        }

        // 개행 문자(\n) 제거
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if (strlen(input) == 0) continue;

        // 명령어 처리를 위해 복사본 생성 (strtok가 원본을 훼손하므로)
        char cmd_buf[256];
        strcpy_s(cmd_buf, sizeof(cmd_buf), input);

        // 첫 번째 토큰 확인 (set, print, exit 등)
        char* token = strtok(cmd_buf, " ");

        if (token == NULL) continue;

        if (strcmp(token, "exit") == 0) {
            break;
        }
        else if (strcmp(token, "print") == 0) {
            print_all_vars();
        }
        else if (strcmp(token, "set") == 0) {
            char* var = strtok(NULL, " ");
            char* val_str = strtok(NULL, " ");
            if (var && val_str) {
                set_var(var, atof(val_str));
                printf("Set %s = %s\n", var, val_str);
            }
            else {
                printf("Usage: set <var> <value>\n");
            }
        }
        else {
            // 명령어가 아니면 수식으로 간주하고 전체 문자열을 eval로 넘김
            double result = evaluate(input);
            set_ret(result);
            printf("= %.6g\n", result);
        }
    }

    return 0;
}