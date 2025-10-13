#ifndef COLORS_H
#define COLORS_H

#include <stdlib.h>
#include <string.h>

// 检测终端是否支持ANSI转义序列
static inline int supports_ansi() {
    const char* term = getenv("TERM");
    if (!term) return 0;
    
    // 检查是否为不支持ANSI的终端类型
    if (strcmp(term, "dumb") == 0) return 0;
    if (strcmp(term, "unknown") == 0) return 0;
    
    // 检查是否明确禁用了颜色
    const char* no_color = getenv("NO_COLOR");
    if (no_color && strlen(no_color) > 0) return 0;
    
    return 1;
}

// 条件性颜色定义
#define COLOR_RESET   (supports_ansi() ? "\x1B[0m" : "")
#define COLOR_RED     (supports_ansi() ? "\x1B[31m" : "")
#define COLOR_GREEN   (supports_ansi() ? "\x1B[32m" : "")
#define COLOR_YELLOW  (supports_ansi() ? "\x1B[33m" : "")
#define COLOR_BLUE    (supports_ansi() ? "\x1B[34m" : "")

// 条件性清屏
#define CLEAR_SCREEN (supports_ansi() ? "\x1B[2J\x1B[H" : "\n")

#endif // COLORS_H
