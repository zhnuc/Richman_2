#include "utils.h"
#include <stdio.h>

void wait_for_enter() {
    printf("\n按 Enter 键继续...");
    // 清除输入缓冲区直到换行符
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
