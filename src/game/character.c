#include "character.h"
#include <stdio.h>
#include <string.h>

// 全局角色数组定义
Character g_characters[4];

void init_characters(void) {
    // 初始化4个预设角色
    g_characters[0].id = 1;
    strcpy(g_characters[0].name, "钱夫人");
    strcpy(g_characters[0].display_name, "钱夫人");
    
    g_characters[1].id = 2;
    strcpy(g_characters[1].name, "阿土伯");
    strcpy(g_characters[1].display_name, "阿土伯");
    
    g_characters[2].id = 3;
    strcpy(g_characters[2].name, "孙小美");
    strcpy(g_characters[2].display_name, "孙小美");
    
    g_characters[3].id = 4;
    strcpy(g_characters[3].name, "金贝贝");
    strcpy(g_characters[3].display_name, "金贝贝");
}

void show_character_selection(void) {
    printf("欢迎来到大富翁，请按数字键选择你的角色：\n");
    for (int i = 0; i < 4; i++) {
        printf("%d.%s\n", g_characters[i].id, g_characters[i].display_name);
    }
}

Character* get_character_by_id(int id) {
    for (int i = 0; i < 4; i++) {
        if (g_characters[i].id == id) {
            return &g_characters[i];
        }
    }
    return NULL;
}

Character* get_character_by_index(int index) {
    if (index >= 0 && index < 4) {
        return &g_characters[index];
    }
    return NULL;
}

bool is_valid_character_id(int id) {
    return id >= 1 && id <= 4;
}