#include "io/command_processor.h"
#include <string.h>
#include <stddef.h>

#ifndef TESTING
int main(int argc, char* argv[]) {
    const char* preset_file = NULL;
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            preset_file = argv[i + 1];
            i++; // 跳过下一个参数
        }
    }
    
    if (preset_file) {
        run_game_with_preset(preset_file);
    } else {
        run_game();
    }
    return 0;
}
#endif