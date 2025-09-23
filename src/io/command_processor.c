#include "command_processor.h"
#include "../game/game_state.h"
#include "../game/player.h"
#include "../game/character.h"
#include "json_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

int get_initial_fund(void) {
    char input[20];
    printf("请输入初始资金: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }
    
    // 去掉换行符
    input[strcspn(input, "\n")] = 0;
    
    int fund = atoi(input);
    if (fund > 0) {
        printf("初始资金设置为: %d\n", fund);
        return fund;
    } else {
        printf("无效的初始资金\n");
        return 0;
    }
}

void show_welcome_and_select_character(int initial_fund) {
    init_characters();
    
    while (g_game_state.player_count == 0) {
        show_character_selection();
        
        char input[10];
        printf("请输入选择 (1-4): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return;
        }
        
        // 去掉换行符
        input[strcspn(input, "\n")] = 0;
        
        // 支持多角色选择，如"12"、"123"、"1234"
        // 首先验证输入是否只包含1-4的数字，且无重复
        bool valid_input = true;
        bool used[5] = {false}; // 检查重复
        
        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] < '1' || input[i] > '4') {
                valid_input = false;
                break;
            }
            int digit = input[i] - '0';
            if (used[digit]) {
                valid_input = false; // 重复数字
                break;
            }
            used[digit] = true;
        }
        
        if (!valid_input) {
            printf("无效选择，请输入1-4之间的数字\n");
            continue;
        }
        
        int player_count = 0;
        bool selected[5] = {false}; // 标记已选择的角色，避免重复
        
        for (int i = 0; input[i] != '\0'; i++) {
            int choice = input[i] - '0';
            if (!selected[choice]) { // 避免重复选择
                selected[choice] = true;
                    Player* player = create_player_by_character(choice, initial_fund);
                if (player) {
                    player_count++;
                    if (player_count == 1) {
                        printf("您选择了: %s", g_characters[choice-1].display_name);
                    } else {
                        printf(", %s", g_characters[choice-1].display_name);
                    }
                }
            }
        }
        
        if (player_count > 0) {
            printf("\n游戏开始！\n");
            break;
        } else {
            printf("无效选择，请输入1-4之间的数字\n");
        }
    }
}

void run_game(void) {
    printf("大富翁游戏启动\n");
    
    init_game_state();
    
    // 检查是否有preset文件
    if (load_game_preset("preset.json") == 0) {
        printf("使用预设配置\n");
    } else {
        // 设置初始资金
        int initial_fund = get_initial_fund();
        if (initial_fund <= 0) {
            printf("无效的初始资金，游戏结束\n");
            return;
        }
        
        // 显示欢迎界面并选择角色
        show_welcome_and_select_character(initial_fund);
        
        // 如果没有任何玩家，直接退出
        if (g_game_state.player_count == 0) {
            printf("没有选择任何角色，游戏结束\n");
            return;
        }
    }
    
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