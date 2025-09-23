#include "command_processor.h"
#include "../game/game_state.h"
#include "../game/player.h"
#include "../game/character.h"
#include "json_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_command(const char* command) {
    if (strcmp(command, "quit") == 0) {
        printf("游戏结束\n");
        return;
    } else if (strncmp(command, "create_player", 13) == 0) {
        // 简单的创建玩家命令: create_player 张三 1500
        char name[32];
        int fund;
        if (sscanf(command, "create_player %s %d", name, &fund) == 2) {
            Player* p = create_player(g_game_state.player_count, name, fund);
            if (p) {
                printf("创建玩家成功: %s\n", p->name);
            } else {
                printf("创建玩家失败\n");
            }
        } else {
            printf("格式错误，请使用: create_player <姓名> <资金>\n");
        }
    } else if (strcmp(command, "status") == 0) {
        print_game_state();
    } else if (strcmp(command, "dump") == 0) {
        // dump命令: 默认保存为dump.json
        save_game_dump("dump.json");
        printf("游戏状态已保存到: dump.json\n");
    } else if (strncmp(command, "dump ", 5) == 0) {
        // dump命令: dump filename.json (带文件名)
        char filename[256];
        if (sscanf(command, "dump %s", filename) == 1) {
            save_game_dump(filename);
            printf("游戏状态已保存到: %s\n", filename);
        } else {
            printf("格式错误，请使用: dump 或 dump <文件名>\n");
        }
    } else if (strncmp(command, "load", 4) == 0) {
        // load命令: load filename.json
        char filename[256];
        if (sscanf(command, "load %s", filename) == 1) {
            if (load_game_preset(filename) == 0) {
                printf("游戏状态已从 %s 加载\n", filename);
            } else {
                printf("加载失败: %s\n", filename);
            }
        } else {
            printf("格式错误，请使用: load <文件名>\n");
        }
    } else {
        printf("未知命令: %s\n", command);
        printf("可用命令: create_player <姓名> <资金>, status, dump, dump <文件名>, load <文件名>, quit\n");
    }
}

void show_welcome_and_select_character(void) {
    init_characters();
    show_character_selection();
    
    char input[10];
    printf("请输入选择 (1-4): ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return;
    }
    
    // 去掉换行符
    input[strcspn(input, "\n")] = 0;
    
    int choice = atoi(input);
    if (is_valid_character_id(choice)) {
        Player* player = create_player_by_character(choice, 1500);
        if (player) {
            printf("您选择了: %s\n", player->name);
            printf("游戏开始！\n");
        } else {
            printf("创建角色失败，请重新选择\n");
            show_welcome_and_select_character();
        }
    } else {
        printf("无效选择，请输入1-4之间的数字\n");
        show_welcome_and_select_character();
    }
}

void run_game(void) {
    printf("大富翁游戏启动\n");
    
    init_game_state();
    
    // 显示欢迎界面并选择角色
    show_welcome_and_select_character();
    
    // 简单的命令行界面
    char command[100];
    while (1) {
        printf("> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // 去掉换行符
        command[strcspn(command, "\n")] = 0;
        
        process_command(command);
        
        // 如果是quit命令，退出循环
        if (strcmp(command, "quit") == 0) {
            break;
        }
    }
}