#include "command_processor.h"
#include "../game/game_state.h"
#include "../game/player.h"
#include "../game/character.h"
#include "../game/map.h"
#include "../game/land.h"
#include "../game/block_system.h"
#include "../io/colors.h"
#include "json_serializer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

// 函数声明
void handle_roll_command();
void handle_step_command(const char* command);
void handle_query_command();
void handle_help_command();
void handle_quit_command();
void handle_sell_command(int location);
void switch_to_next_player();


void process_command(const char* command) {
    char lower_command[100];
    for (int i = 0; command[i]; i++) {
        lower_command[i] = tolower(command[i]);
    }
    lower_command[strlen(command)] = '\0';

    if (strcmp(lower_command, "roll") == 0) {
        handle_roll_command();
    } else if (strncmp(lower_command, "step", 4) == 0) {
        handle_step_command(lower_command);
    } else if (strcmp(lower_command, "query") == 0) {
        handle_query_command();
    } else if (strcmp(lower_command, "help") == 0) {
        handle_help_command();
    } else if (strncmp(lower_command, "sell ", 5) == 0) {
        int location;
        if (sscanf(command, "sell %d", &location) == 1) {
            handle_sell_command(location);
        } else {
            printf("格式错误，请使用: sell <位置>\n");
        }
    } else if (strncmp(lower_command, "block ", 6) == 0) {
        int relative_distance;
        if (sscanf(command, "block %d", &relative_distance) == 1) {
            Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
            handle_block_command(current_player, relative_distance);
        } else {
            printf("格式错误，请使用: block <相对距离>\n");
        }
    } else if (strncmp(lower_command, "bomb ", 5) == 0) {
        int relative_distance;
        if (sscanf(command, "bomb %d", &relative_distance) == 1) {
            Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
            handle_bomb_command(current_player, relative_distance);
        } else {
            printf("格式错误，请使用: bomb <相对距离>\n");
        }
    } else if (strcmp(lower_command, "robot") == 0) {
        Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
        handle_robot_command(current_player);
    } else if (strcmp(lower_command, "quit") == 0) {
        handle_quit_command();
    } else if (strncmp(lower_command, "create_player", 13) == 0) {
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
    } else if (strcmp(lower_command, "status") == 0) {
        print_game_state();
    } else if (strcmp(lower_command, "dump") == 0) {
        // dump命令: 默认保存为dump.json
        save_game_dump("dump.json");
        printf("游戏状态已保存到: dump.json\n");
    } else if (strncmp(lower_command, "dump ", 5) == 0) {
        // dump命令: dump filename.json (带文件名)
        char filename[256];
        if (sscanf(command, "dump %s", filename) == 1) {
            save_game_dump(filename);
            printf("游戏状态已保存到: %s\n", filename);
        } else {
            printf("格式错误，请使用: dump 或 dump <文件名>\n");
        }
    } else if (strncmp(lower_command, "load", 4) == 0) {
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
        handle_help_command();
    }
}

void handle_roll_command() {
    Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
    
    int steps = rand() % 6 + 1;
    printf("玩家 %s 掷骰子，点数为 %d\n", current_player->name, steps);
    
    int original_location = current_player->location;
    
    // 检查移动路径上是否有路障或炸弹
    for (int i = 1; i <= steps; i++) {
        int next_location = (original_location + i) % MAP_SIZE;
        
        // 检查路障拦截
        if (check_block_interception(next_location)) {
            // 被路障拦截，停留在路障所在位置
            current_player->location = next_location;
            printf("%s 前进 %d 步，到达位置 %d\n", current_player->name, i, current_player->location);
            trigger_block_interception(current_player, next_location);
            
            // 即使被拦截也要切换到下一个玩家
            switch_to_next_player();
            return;
        }
        
        // 检查炸弹爆炸
        if (check_bomb_explosion(next_location)) {
            // 踩中炸弹，触发爆炸效果
            printf("%s 前进 %d 步，踩中位置 %d 的炸弹\n", current_player->name, i, next_location);
            trigger_bomb_explosion(current_player, next_location);
            
            // 即使被炸也要切换到下一个玩家
            switch_to_next_player();
            return;
        }
    }
    
    // 没有路障拦截，正常移动
    current_player->location = (current_player->location + steps) % MAP_SIZE;
    printf("%s 前进 %d 步，到达位置 %d\n", current_player->name, steps, current_player->location);

    // 触发到达事件
    on_player_land(current_player);

    // 切换到下一个玩家
    switch_to_next_player();
}

void handle_step_command(const char* command) {
    int steps;
    if (sscanf(command, "step %d", &steps) == 1) {
        Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
        
        printf("遥控骰子，指定步数为 %d\n", steps);
        int original_location = current_player->location;
        
        // 检查移动路径上是否有路障或炸弹
        for (int i = 1; i <= steps; i++) {
            int next_location = (original_location + i) % MAP_SIZE;
            
            // 检查路障拦截
            if (check_block_interception(next_location)) {
                // 被路障拦截，停留在路障所在位置
                current_player->location = next_location;
                printf("%s 前进 %d 步，到达位置 %d\n", current_player->name, i, current_player->location);
                trigger_block_interception(current_player, next_location);
                
                // 即使被拦截也要切换到下一个玩家
                switch_to_next_player();
                return;
            }
            
            // 检查炸弹爆炸
            if (check_bomb_explosion(next_location)) {
                // 踩中炸弹，触发爆炸效果
                printf("%s 前进 %d 步，踩中位置 %d 的炸弹\n", current_player->name, i, next_location);
                trigger_bomb_explosion(current_player, next_location);
                
                // 即使被炸也要切换到下一个玩家
                switch_to_next_player();
                return;
            }
        }
        
        // 没有路障拦截，正常移动
        if (steps > 0) {
            current_player->location = (current_player->location + steps) % MAP_SIZE;
            printf("%s 前进 %d 步，到达位置 %d\n", current_player->name, steps, current_player->location);
            // 触发到达事件
            on_player_land(current_player);
        } else {
            printf("%s 前进 %d 步，到达位置 %d\n", current_player->name, steps, current_player->location);
        }

        // 切换到下一个玩家
        switch_to_next_player();
    } else {
        printf("无效的 step 命令格式, e.g., step 5\n");
    }
}

void handle_query_command() {
    Player* p = &g_game_state.players[g_game_state.game.now_player_id];
    printf("资产查询:\n");
    printf("  玩家: %s\n", p->name);
    printf("  资金: %d 元\n", p->fund);
    printf("  点数: %d 点\n", p->credit);
    printf("  位置: %d\n", p->location);
    printf("  道具:\n");
    printf("    - 炸弹: %d\n", p->prop.bomb);
    printf("    - 路障: %d\n", p->prop.barrier);
    printf("    - 机器娃娃: %d\n", p->prop.robot);
    printf("  房产:\n");
    bool has_house = false;
    for (int i = 0; i < MAP_SIZE; i++) {
        if (g_game_state.houses[i].owner_id == p->index) {
            printf("    - 位置 %d (等级 %d)\n", i, g_game_state.houses[i].level);
            has_house = true;
        }
    }
    if (!has_house) {
        printf("    (无)\n");
    }
}

void handle_help_command() {
    printf("命令帮助:\n");
    printf("roll\n");
    printf("  掷骰子命令，行走1～6步，步数由随机算法产生。\n");
    printf("sell n\n");
    printf("  出售房产，n为房产在地图上的绝对位置，出售价格为投资总成本的2倍。\n");
    printf("block n\n");
    printf("  放置路障，n为前后相对距离（±10步），玩家经过将被拦截。\n");
    printf("bomb n\n");
    printf("  放置炸弹，n为前后相对距离（±10步），玩家经过将被炸伤，住院3天。\n");
    printf("robot\n");
    printf("  清扫前方10步内的障碍（路障、炸弹）。\n");
    printf("query\n");
    printf("  显示自家资产信息。\n");
    printf("help\n");
    printf("  查看命令帮助。\n");
    printf("quit\n");
    printf("  强制退出游戏。\n");
    printf("step n\n");
    printf("  遥控骰子，指定行走步数。\n");
}

void handle_quit_command() {
        g_game_state.game.ended = true;
        printf("游戏结束。\n");
}


int get_initial_fund(void) {
    char input[20];
    int fund = 10000; // 默认资金
    printf("请设置玩家初始资金（范围：1000～50000，默认10000），直接回车使用默认资金: ");
    if (fgets(input, sizeof(input), stdin) != NULL && input[0] != '\n') {
        int temp_fund = atoi(input);
        if (temp_fund >= 1000 && temp_fund <= 50000) {
            fund = temp_fund;
        } else {
            printf("无效的资金数额，将使用默认资金 10000。\n");
        }
    }
    printf("初始资金设置为: %d\n", fund);
    return fund;
}

void show_welcome_and_select_character(int initial_fund) {
    init_characters();
    
    while (g_game_state.player_count < 2 || g_game_state.player_count > 4) {
        show_character_selection();
        
        char input[10];
        printf("请选择2～4位不重复玩家，输入编号即可（1、钱夫人；2、阿土伯；3、孙小美；4、金贝贝）: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        bool valid_input = true;
        bool used[5] = {false};
        int len = strlen(input);
        if (len < 2 || len > 4) {
            printf("请选择 2-4 位玩家。\n");
            continue;
        }
        
        for (int i = 0; i < len; i++) {
            if (input[i] < '1' || input[i] > '4') {
                valid_input = false;
                break;
            }
            int digit = input[i] - '0';
            if (used[digit]) {
                printf("角色选择重复。\n");
                valid_input = false; 
                break;
            }
            used[digit] = true;
        }
        
        if (!valid_input) {
            printf("无效选择，请输入1-4之间的不重复数字。\n");
            g_game_state.player_count = 0; // 重置玩家计数
            continue;
        }
        
        for (int i = 0; i < len; i++) {
            int choice = input[i] - '0';
            Player* player = create_player_by_character(choice, initial_fund);
            if (player) {
                 printf("玩家 %s (%s) 加入游戏。\n", player->name, get_character_by_id(choice)->display_name);
            }
        }
        
        if (g_game_state.player_count > 0) {
            printf("\n游戏开始！\n");
            g_game_state.game.started = true;
            break;
        }
    }
}

void run_game(void) {
    printf("大富翁游戏启动\n");
    srand(time(NULL)); // 初始化随机数种子
    
    init_game_state();
    bool game_started = false;
    
    if (load_game_preset("preset.json") == 0) {
        printf("使用预设配置\n");
    } else {
        int initial_fund = get_initial_fund();
        show_welcome_and_select_character(initial_fund);
        
        if (g_game_state.player_count == 0) {
            printf("没有选择任何角色，游戏结束\n");
            return;
        }
        game_started = true;
    }
    
    char command[100];
    char last_command_output[1024] = {0}; // 用于存储上一条命令的输出

    // 初始显示
    printf(CLEAR_SCREEN);
    display_map();

    while (!g_game_state.game.ended) {
        // 检查胜利条件
        if (game_started) {
            int alive_count = 0;
            int winner_id = -1;
            for (int i = 0; i < g_game_state.player_count; i++) {
                if (g_game_state.players[i].alive) {
                    alive_count++;
                    winner_id = i;
                }
            }

            if (alive_count <= 1) {
                printf(CLEAR_SCREEN);
                display_map();
                if (winner_id != -1) {
                    printf("游戏结束！胜利者是 %s！\n", g_game_state.players[winner_id].name);
                } else {
                    printf("所有玩家都已破产，游戏结束！\n");
                }
                g_game_state.game.ended = true;
                //wait_for_enter();
                continue;
            }
        }

        Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];

        // 检查财神附身状态
        if (current_player->buff.god > 0) {
            printf("玩家 %s 有财神附身，免过路费，剩余 %d 回合。\n",
                   current_player->name, current_player->buff.god);
            current_player->buff.god--;
        }

        // 检查当前玩家是否已破产，如果是则自动跳过
        if (!current_player->alive) {
            switch_to_next_player();
            continue; // 直接进入下一位玩家
        }
        
        // 检查当前玩家是否在住院，如果是则自动跳过
        if (current_player->buff.hospital > 0) {
            printf("玩家 %s 正在住院治疗，剩余 %d 天，本轮自动跳过。\n", 
                   current_player->name, current_player->buff.hospital);
            current_player->buff.hospital--;
            switch_to_next_player();
            //wait_for_enter();
            continue; // 直接进入下一轮
        }
        
        // 检查当前玩家是否在监狱，如果是则自动跳过
        if (current_player->buff.prison > 0) {
            printf("玩家 %s 正在监狱中，剩余 %d 天，本轮自动跳过。\n", 
                   current_player->name, current_player->buff.prison);
            current_player->buff.prison--;
            switch_to_next_player();
            //wait_for_enter();
            continue; // 直接进入下一轮
        }
        
        
        printf("%s%c%s> ", current_player->color, current_player->name[0], COLOR_RESET);
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Trim trailing newline
        command[strcspn(command, "\n")] = 0;
        
        // 在处理命令前清屏并重绘
        printf(CLEAR_SCREEN);
        display_map();

        process_command(command);

        if (!g_game_state.game.ended) {
            //wait_for_enter();
        }
    }
}

void switch_to_next_player() {
    g_game_state.game.now_player_id = (g_game_state.game.now_player_id + 1) % g_game_state.player_count;
    g_game_state.game.next_player_id = (g_game_state.game.now_player_id + 1) % g_game_state.player_count;
}