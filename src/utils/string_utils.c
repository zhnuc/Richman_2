#include "string_utils.h"
#include <string.h>
#include <stdbool.h>

void trim_newline(char* str) {
    if (str == NULL) return;
    str[strcspn(str, "\n")] = 0;
}

bool is_empty_string(const char* str) {
    return str == NULL || str[0] == '\0';
}